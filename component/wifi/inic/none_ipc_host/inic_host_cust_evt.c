/*
 *  Routines to send and receive data between host and device
 *
 *  Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "inic_host_cust_evt.h"

/**
* @brief  This is a function implemented by customer. It shuld to used to
* receive the data from device.
* @param  buf: the buffer sent to device from host.
* @param  len: length of the transmitted data.
* @return none.
*/
static inline void inic_host_recv_cust_evt_hdl(u8 *buf, u32 len)
{
	(void) buf;
	(void) len;

	/* implemented by customer */

	return;
}


void inic_host_recv_cust_evt(u8 *rx_buf)
{
	struct inic_cust_hdr *head = (struct inic_cust_hdr *)rx_buf;
	u8 *buf = rx_buf + sizeof(struct inic_cust_hdr);

	inic_host_recv_cust_evt_hdl(buf, head->len);

	return;
}

/**
* @brief  This is a function to send the date to host from device.
* @param  buf_in: the buffer sent to host from device.
* @param  len: length of the transmitted data.
* @return none.
*/
void inic_host_send_cust_evt(u8 *buf_in, u32 len)
{
	u8 *buf = NULL;
	struct inic_cust_hdr *head = NULL;
	struct inic_txbuf_info_t *inic_tx = NULL;

	buf = rtos_mem_zmalloc(sizeof(struct inic_cust_hdr) + len + DEV_DMA_ALIGN);
	if (!buf) {
		return;
	}
	head = (struct inic_cust_hdr *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);

	/* notify NP that event is finished */
	head->event = INIC_CUST_EVT;
	head->len = len;
	/* construct struct inic_buf_info & inic_buf_info_t */
	inic_tx = (struct inic_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct inic_txbuf_info_t));
	if (!inic_tx) {
		rtos_mem_free(buf);
		return;
	}
	memcpy((void *)(head + 1), buf_in, len);

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)head;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = sizeof(struct inic_cust_hdr) + len;

	inic_tx->ptr = buf;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	inic_host_send_data(&inic_tx->txbuf_info);
}