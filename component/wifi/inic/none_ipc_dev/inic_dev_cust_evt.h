/*
 *  Routines to send and receive data between host and device
 *
 *  Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _INIC_DEV_CUST_EVT_H_
#define _INIC_DEV_CUST_EVT_H_

void inic_dev_recv_cust_evt(u8 *buf);
void inic_dev_send_cust_evt(u8 *buf, u32 len);

#endif