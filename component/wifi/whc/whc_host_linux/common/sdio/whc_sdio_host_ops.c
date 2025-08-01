#include <whc_host_linux.h>
#include <whc_host_cmd_path_api.h>

static struct sk_buff *whc_sdio_host_read_rxfifo(struct whc_sdio *priv, u32 size)
{
	u32 allocsize, ret;
	u32 retry = 0;
	struct sk_buff *pskb = NULL;

	allocsize = _RND(size, priv->func->cur_blksize);

	pskb = netdev_alloc_skb(NULL, allocsize);
	if (pskb == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: Alloc skb rx buf Err, alloc_sz %d!!\n\r",
				__func__, allocsize);
		return NULL;
	}

	while (1) {
		ret = sdio_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, pskb->data);
		if (ret == true) {
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry++ > 10) {
				kfree_skb(pskb);
				dev_err(&priv->func->dev, "%s: read port FAIL!\n", __FUNCTION__);
				return NULL;
			};
		}
	}

	//print_hex_dump_bytes("whc_sdio_host_read_rxfifo: ", DUMP_PREFIX_NONE, pskb->data, size);

	return pskb;
}

void whc_sdio_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb)
{
	u32 polling_num = 0, try_cnt = 0;
	struct whc_sdio *priv = &whc_sdio_priv;
	INIC_TX_DESC *ptxdesc;

	(void) polling_num;
	(void) pskb;

	/* wakeup device if it's in power save mode before send msg */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		dev_dbg(&priv->func->dev, "%s: wakeup device", __func__);
		if (whc_sdio_host_resume_common(priv)) {
			dev_err(&priv->func->dev, "%s: fail to wakeup device, stop send", __func__);
			return;
		}
	}

	mutex_lock(&priv->lock);

	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
		/* set 0 after query num < 1 */
		/* jira: https://jira.realtek.com/browse/RSWLANDIOT-11649 */
		priv->tx_avail_int_triggered = 0;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		if (try_cnt ++ > 0) {
			if (!wait_event_timeout(priv->txbd_wq, priv->tx_avail_int_triggered == 1, msecs_to_jiffies(1000))) {
				dev_err(&priv->func->dev, "%s: TXBD unavailable, TX FAIL\n", __FUNCTION__);
				goto exit;
			}
		}
		rtw_sdio_query_txbd_status(priv);
#else
		polling_num++;
		if ((polling_num % 60) == 0) {
			msleep(1);
		}

		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
#endif
	}

	if (len > priv->SdioTxMaxSZ) {
		dev_err(&priv->func->dev, "%s: PKT SIZE ERROR, total size: %d\n", __FUNCTION__, len);
		goto exit;
	}

	ptxdesc = (INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	//print_hex_dump_bytes("whc_sdio_host_send_data: ", DUMP_PREFIX_NONE, buf, len);

	rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, len, buf);

	if (priv->SdioTxBDFreeNum > 0) {
		priv->SdioTxBDFreeNum -= 1;
	}

	/* update txbd num in sw after send one pkt */
	priv->txbd_wptr = (priv->txbd_wptr + 1) % priv->txbd_size;
exit:
	mutex_unlock(&priv->lock);

	return;
}

void whc_sdio_host_recv_data_process(void *intf_priv)
{
	struct whc_sdio *sdio_priv = (struct whc_sdio *)intf_priv;
	u8 tmp[4];
	struct sk_buff *pskb;
	u32 rx_len_rdy, himr;
	u16 SdioRxFIFOSize;
	u8 retry = 0;

	/* wakeup device if it's sleep */
	if (sdio_priv->dev_state == PWR_STATE_SLEEP) {
		dev_dbg(&sdio_priv->func->dev, "%s: wakeup device", __func__);
		if (whc_sdio_host_resume_common(sdio_priv)) {
			dev_err(&sdio_priv->func->dev, "%s: fail to wakeup device, stop read rxfifo", __func__);
			return;
		}
	}

	do {
		/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
		rx_len_rdy = sdio_read8(sdio_priv, SDIO_REG_RX0_REQ_LEN + 3) & BIT(7);

		if (rx_len_rdy) {
			sdio_local_read(sdio_priv, SDIO_REG_RX0_REQ_LEN, 4, tmp);
			SdioRxFIFOSize = le16_to_cpu(*(u16 *)tmp);

			if (SdioRxFIFOSize == 0) {
				if (retry ++ < 3) {
					continue;
				} else {
					break;
				}
			} else {
				retry = 0;
				pskb = whc_sdio_host_read_rxfifo(sdio_priv, SdioRxFIFOSize);
				if (pskb) {
					/* skip RX_DESC */
					sdio_priv->rx_process_func(pskb);
				} else {
					break;
				}
			}
		} else {
			break;
		}
	} while (1);

	/* restore RX_REQ interrupt*/
	himr = cpu_to_le32(sdio_priv->sdio_himr);
	sdio_local_write(sdio_priv, SDIO_REG_HIMR, 4, (u8 *)&himr);
}

#ifdef CONFIG_WHC_CMD_PATH
void whc_host_send_cmd_data(u8 *buf, u32 len)
{
	whc_sdio_host_send_data(buf, len, NULL);
}

int whc_host_cmd_data_rx_to_user(struct sk_buff *pskb)
{
	int ret = 0;
	u16 size;

	/* size after padding for align */
	size = *(u16 *)pskb->data;
	ret = whc_host_buf_rx_to_user((u8 *)pskb->data + SIZE_RX_DESC, size);

	return ret;
}
#endif

struct hci_ops_t whc_sdio_host_intf_ops = {
	.send_data = whc_sdio_host_send_data,
	.recv_data_process = whc_sdio_host_recv_data_process,
};

