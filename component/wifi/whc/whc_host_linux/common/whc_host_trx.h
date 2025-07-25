#ifndef __RTW_LLHW_TRX_H__
#define __RTW_LLHW_TRX_H__

/*TX reserve size before 802.3 pkt*/
#define WLAN_ETHHDR_LEN		14
#define TXDESC_SIZE		40
#define WLAN_HDR_A4_QOS_HTC_LEN	36
#define WLAN_MAX_IV_LEN		8
#define WLAN_SNAP_HEADER	8
#define WLAN_MAX_MIC_LEN	8
#define WLAN_MAX_ICV_LEN	8

#define MAXIMUM_ETHERNET_PACKET_SIZE		1514
#define SKB_WLAN_TX_EXTRA_LEN	(TXDESC_SIZE+WLAN_HDR_A4_QOS_HTC_LEN+WLAN_MAX_IV_LEN+WLAN_SNAP_HEADER-WLAN_ETHHDR_LEN)
#define WLAN_MAX_PROTOCOL_OVERHEAD (WLAN_HDR_A4_QOS_HTC_LEN+WLAN_MAX_IV_LEN+\
							WLAN_SNAP_HEADER+WLAN_MAX_MIC_LEN+WLAN_MAX_ICV_LEN)/*=68*/

/* should  sync with autoconf_8730e.h */
#define SKB_CACHE_SZ 64
#define SKB_ALIGNMENT	__attribute__((aligned(SKB_CACHE_SZ)))
#define WLAN_HW_INFO_LEN	40/*max(TXDESC_SIZE, RXDESC_SIZE+RXPHYSTS_SIZE)*/
#define WLAN_MAX_ETHFRM_LEN	1904/*max payload size of wifi frame*/

#define MAX_SKB_BUF_SIZE	(((WLAN_HW_INFO_LEN+WLAN_MAX_PROTOCOL_OVERHEAD+WLAN_MAX_ETHFRM_LEN+8)\
							+ SKB_CACHE_SZ) & ~(SKB_CACHE_SZ-1))

#define MAX_LENGTH_OF_TX_QUEUE	(200)
#define MAX_TIMES_TO_TRY_TX	(5)

#define PKT_DROP_THRES		10
#define QUEUE_STOP_THRES	7
#define QUEUE_WAKE_THRES	4

#ifndef CONFIG_FULLMAC_HCI_IPC
/* internal pkt tx: from user space to dev by cmd path */
void whc_host_send_cmd_data(u8 *buf, u32 len);

/* internal pkt rx: from dev to host kernel space */
int whc_host_cmd_data_rx_to_user(struct sk_buff *pskb);
#endif

#endif /* __RTW_LLHW_TRX_H__ */
