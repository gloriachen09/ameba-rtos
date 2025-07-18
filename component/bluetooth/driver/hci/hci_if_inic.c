/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "osif.h"
#include "hci_if_inic.h"
#include "hci_transport.h"
#include "hci_uart.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "hci_common.h"
#include "bt_inic.h"

uint8_t hci_process(void);

void bt_inic_send_cmd_complete_evt(uint16_t opcode, uint8_t status)
{
	uint8_t evt[6] = {0x0e, 0x04, 0x03, 0x00, 0x00, 0x00};
	evt[3] = opcode & 0xFF;
	evt[4] = (opcode >> 8) & 0xFF;
	evt[5] = status;
	bt_inic_send_to_host(HCI_EVT, evt, sizeof(evt));
}

#if defined(BT_INIC_FPGA_VERIFICATION) && BT_INIC_FPGA_VERIFICATION
static uint8_t rd_local_name[] = {
	0x0e, 0xfc, 0x01, 0x14, 0x0c, 0x00,
	0x41, 0x6d, 0x65, 0x62, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8_t rd_class_of_device[] = {0x0e, 0x07, 0x02, 0x23, 0x0c, 0x00, 0x00, 0x00, 0x00};
static uint8_t rd_voice_setting[] = {0x0e, 0x06, 0x02, 0x25, 0x0c, 0x00, 0x00, 0x00};
static uint8_t rd_number_of_supported_IAC[] = {0x0e, 0x05, 0x02, 0x38, 0x0c, 0x00, 0x01};
static uint8_t rd_current_IAC_LAP[] = {0x0e, 0x06, 0x02, 0x39, 0x0c, 0x00, 0x01, 0x03};
static uint8_t rd_inquing_rsp_pwlvl[] = {0x0e, 0x05, 0x02, 0x58, 0x0c, 0x00, 0x08};
static uint8_t rd_local_version[] = {0x0e, 0x0c, 0x02, 0x01, 0x10, 0x00, 0x0c, 0xfa, 0x05, 0x0c, 0x5d, 0x00, 0x77, 0x79};
static uint8_t rd_local_feature[] = {0x0e, 0x0c, 0x02, 0x03, 0x10, 0x00, 0xff, 0xff, 0xff, 0xfe, 0xdb, 0xfd, 0x7b, 0x87};
static uint8_t rd_local_ext_feature[16] = {0x0e, 0x0e, 0x02, 0x04, 0x10, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t rd_buffsize[] = {0x0e, 0x0b, 0x02, 0x05, 0x10, 0x00, 0xfd, 0x03, 0xff, 0x08, 0x00, 0x0c, 0x00};
static uint8_t rd_bd_addr[] = {0x0e, 0x0a, 0x02, 0x09, 0x10, 0x00, 0x8c, 0x06, 0x80, 0x4c, 0xe0, 0x00};
static uint8_t rd_local_cmds[] = {
	0x0e, 0x44, 0x02, 0x02, 0x10, 0x00, 0xff, 0xff, 0xff, 0x03, 0xcc, 0xff,
	0xff, 0xff, 0x3f, 0xff, 0xfc, 0x9f, 0xf3, 0x0f, 0xe8, 0xfe, 0x3f, 0xf7,
	0x8f, 0xff, 0x1c, 0x00, 0x00, 0x00, 0x61, 0xf7, 0xff, 0xff, 0x7f, 0x20,
	0x00, 0x00, 0xfe, 0xc0, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xe, 0xff, 0xeb,
	0xff, 0xff, 0xff, 0x9d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8_t rd_buffer_size[] = {0x0e, 0x07, 0x02, 0x02, 0x20, 0x00, 0xfb, 0x00, 0x08};
static uint8_t rd_local_feature2[] = {0x0e, 0x0c, 0x02, 0x03, 0x20, 0x00, 0xfd, 0x7f, 0xfe, 0xff, 0x1e, 0x00, 0x00, 0x00};
static uint8_t rd_supported_states[] = {0x0e, 0x0c, 0x02, 0x1c, 0x20, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00};
static uint8_t rd_stored_link_key[] = {0x0e, 0x08, 0x01, 0x0d, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t rd_page_scan_act[] = {0x0e, 0x08, 0x02, 0x1b, 0x0c, 0x00, 0x00, 0x08, 0x12, 0x00};
static uint8_t le_rd_adv_chnl_txpwr[] = {0x0e, 0x05, 0x01, 0x07, 0x20, 0x00, 0x06};
static uint8_t le_rd_accept_list_size[] = {0x0e, 0x05, 0x01, 0x07, 0x20, 0x00, 0x06};
static uint8_t rd_page_scan_type[] = {0x0e, 0x05, 0x01, 0x46, 0x0c, 0x00, 0x00};
static uint8_t del_stored_link_key[] = {0x0e, 0x06, 0x01, 0x12, 0x0c, 0x00, 0x00, 0x00};
static uint8_t rd_local_codecs1[] = {0x0e, 0x05, 0x01, 0x0b, 0x10, 0x00, 0x00};
static uint8_t rd_local_codecs2[] = {0x0e, 0x05, 0x01, 0x0d, 0x10, 0x00, 0x00};
static uint8_t rd_local_simplepairing[] = {0x0e, 0x06, 0x01, 0x0c, 0x10, 0x00, 0x00, 0x07};

struct fake_hci_evt {
	uint16_t opcode;
	uint8_t *evt;
	uint8_t len;
};

static struct fake_hci_evt fake_events[] = {
	{HCI_Read_Local_Name, rd_local_name, sizeof(rd_local_name)},
	{HCI_Read_Class_of_Device, rd_class_of_device, sizeof(rd_class_of_device)},
	{HCI_Read_Voice_Setting, rd_voice_setting, sizeof(rd_voice_setting)},
	{HCI_Read_Number_of_supported_IAC, rd_number_of_supported_IAC, sizeof(rd_number_of_supported_IAC)},
	{HCI_Read_Current_IAC_LAP, rd_current_IAC_LAP, sizeof(rd_current_IAC_LAP)},
	{HCI_Read_Inquing_response_PowerLevel, rd_inquing_rsp_pwlvl, sizeof(rd_inquing_rsp_pwlvl)},
	{HCI_Read_Local_Version, rd_local_version, sizeof(rd_local_version)},
	{HCI_Read_Local_Supported_Feature, rd_local_feature, sizeof(rd_local_feature)},
	{HCI_Read_Local_Extended_Feature, rd_local_ext_feature, sizeof(rd_local_ext_feature)},
	{HCI_Read_Buffsize, rd_buffsize, sizeof(rd_buffsize)},
	{HCI_Read_BD_ADDR, rd_bd_addr, sizeof(rd_bd_addr)},
	{HCI_Read_Local_Supported_Commands, rd_local_cmds, sizeof(rd_local_cmds)},
	{HCI_LE_Read_Buffer_Size, rd_buffer_size, sizeof(rd_buffer_size)},
	{HCI_Read_Local_Supported_Feature_V2, rd_local_feature2, sizeof(rd_local_feature2)},
	{HCI_Read_Support_States, rd_supported_states, sizeof(rd_supported_states)},
	{HCI_Read_Stored_Link_Key, rd_stored_link_key, sizeof(rd_stored_link_key)},
	{HCI_Read_Page_Scan_Activity, rd_page_scan_act, sizeof(rd_page_scan_act)},
	{HCI_LE_Read_Advertising_Channel_Tx_Power, le_rd_adv_chnl_txpwr, sizeof(le_rd_adv_chnl_txpwr)},
	{HCI_LE_Read_Filter_Accept_List_Size, le_rd_accept_list_size, sizeof(le_rd_accept_list_size)},
	{HCI_Read_Page_Scan_Type, rd_page_scan_type, sizeof(rd_page_scan_type)},
	{HCI_Delete_Stored_Link_Key, del_stored_link_key, sizeof(del_stored_link_key)},
	{HCI_Read_Local_Supported_Codecs_v1, rd_local_codecs1, sizeof(rd_local_codecs1)},
	{HCI_Read_Local_Supported_Codecs_v2, rd_local_codecs2, sizeof(rd_local_codecs2)},
	{HCI_Read_Local_Simple_Pairing_Options, rd_local_simplepairing, sizeof(rd_local_simplepairing)},
};

static void _handle_cmd_for_fpga(uint16_t opcode)
{
	BT_LOGA("Host TX hci cmd 0x%02x TO Device\r\n", opcode);
	int i, num = sizeof(fake_events) / sizeof(struct fake_hci_evt);

	for (i = 0; i < num; i++) {
		if (opcode == fake_events[i].opcode) {
			bt_inic_send_to_host(HCI_EVT, fake_events[i].evt, fake_events[i].len);
			return;
		}
	}

	bt_inic_send_cmd_complete_evt(opcode, 0);
}

#else

/************* Receive packet from controller **************/
static uint8_t *new_packet_buf = NULL;
static uint16_t new_packet_len = 0;
static uint8_t new_packet_type = 0;
static void *internal_cmd_q;
static uint32_t internal_cmd_cnt = 0;
static uint8_t *bt_inic_get_buf(uint8_t type, void *hdr, uint16_t len, uint32_t timeout)
{
	(void)hdr;
	(void)timeout;
	uint8_t *buf = NULL;

	buf = (uint8_t *)osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, len, 4);
	memset(buf, 0, len);
	new_packet_buf = buf;
	new_packet_len = len;
	new_packet_type = type;

	return buf;
}

static void bt_inic_cancel(void)
{
	osif_mem_aligned_free(new_packet_buf);
}

/* packet is received from controller, send it to host. */
static void bt_inic_recv(void)
{
	uint16_t opcode, opcode_i;
	uint8_t *buf = new_packet_buf;

	/* filter out event for internal sent cmd */
	if (internal_cmd_cnt) {
		if (new_packet_type == HCI_EVT && *buf == BT_HCI_EVT_CMD_COMPLETE) {
			LE_TO_UINT16(opcode, buf + 3);
			osif_msg_peek(internal_cmd_q, &opcode_i, BT_TIMEOUT_NONE);
			if (opcode == opcode_i) { /* event for internal hci command, no need send to stack */
				/* BT_LOGA("filter out opcode 0x%04x\r\n", opcode); */
				osif_msg_recv(internal_cmd_q, &opcode_i, BT_TIMEOUT_NONE);
				osif_msg_queue_peek(internal_cmd_q, &internal_cmd_cnt);
				osif_mem_aligned_free(buf);
				return;
			}
		}
	}

	bt_inic_send_to_host(new_packet_type, buf, new_packet_len);
	osif_mem_aligned_free(buf);
}

static struct hci_transport_cb bt_inic_cb = {
	.get_buf = bt_inic_get_buf,
	.recv = bt_inic_recv,
	.cancel = bt_inic_cancel,
};
/***********************************************************/

bool bt_inic_open(void)
{
	if (hci_controller_is_enabled()) {
		return true;
	}

	if (!hci_controller_enable()) {
		return false;
	}

	osif_msg_queue_create(&internal_cmd_q, sizeof(uint16_t), 10);

	/* HCI Transport Bridge to BT inic */
	hci_transport_register(&bt_inic_cb);

	return true;
}

void bt_inic_close(void)
{
	hci_controller_disable();
	hci_controller_free();
	internal_cmd_cnt = 0;
	osif_msg_queue_delete(internal_cmd_q);
}

static bool is_inic_vendor_cmd(uint16_t opcode, uint8_t *pdata)
{
	bool is_inic_cmd = true;
	uint8_t status = 0;
	uint8_t param;

	switch (opcode) {
	case BT_HCI_CMD_VS_BT_ON:
		status = bt_inic_open() ? 0 : 1;
		BT_LOGA("BT INIC Open\r\n");
		break;
	case BT_HCI_CMD_VS_BT_OFF:
		bt_inic_close();
		BT_LOGA("BT INIC Close\r\n");
		break;
	case BT_HCI_CMD_VS_BT_HOST_SLEEP:
		BT_LOGA("BT INIC Host sleep\r\n");
		break;
	case BT_HCI_CMD_VS_BT_HOST_RESUME:
		BT_LOGA("BT INIC Host resume\r\n");
		break;
	case BT_HCI_CMD_VS_BT_MP_DOWNLOAD:
		param = pdata[3];
		if (param == 0x01) {
			hci_set_mp(true);
			if (hci_controller_is_enabled()) {
				hci_controller_disable();
				hci_controller_free();
			}
			if (!hci_controller_enable()) {
				BT_LOGE("BT MP Patch download failed!\r\n");
				status = 1; // download patch failed
			} else {
				BT_LOGA("BT MP Patch download success!\r\n");
			}
			hci_transport_register(&bt_inic_cb);
		} else {
			hci_set_mp(false);
			if (hci_controller_is_enabled()) {
				hci_controller_disable();
				hci_controller_free();
			}
			if (!hci_controller_enable()) {
				BT_LOGE("BT Normal Patch download failed!\r\n");
				status = 1; // download patch failed
			} else {
				BT_LOGA("BT Normal Patch download success!\r\n");
			}
			hci_transport_register(&bt_inic_cb);
		}
		break;
	default:
		is_inic_cmd = false;
		break;
	}

	if (is_inic_cmd) {
		bt_inic_send_cmd_complete_evt(opcode, status);
	}

	return is_inic_cmd;
}

#endif

void bt_inic_recv_from_host(uint8_t type, uint8_t *pdata, uint32_t len)
{
	uint16_t opcode;
#if defined(BT_INIC_FPGA_VERIFICATION) && BT_INIC_FPGA_VERIFICATION
	(void)len;

	if (type == HCI_CMD) {
		LE_TO_UINT16(opcode, pdata);
		_handle_cmd_for_fpga(opcode);
		return;
	}
	BT_LOGA("Host TX type(%d) packet TO Device\r\n", type);

#else
	if (type == HCI_CMD) {
		LE_TO_UINT16(opcode, pdata);
		if (is_inic_vendor_cmd(opcode, pdata)) {
			return;
		}
	}

	if (!hci_controller_is_enabled()) {
		BT_LOGA("Controller is off now, Power on.\r\n", type);
		bt_inic_open(); // auto power on
	}
	hci_transport_send(type, pdata, len, false);
#endif
}

bool hci_if_write_internal(uint8_t *buf, uint32_t len)
{
	uint16_t opcode;

	if (!hci_controller_is_enabled()) {
		return false;
	}

	if (*buf == HCI_CMD) {
		LE_TO_UINT16(opcode, buf + 1);
		osif_msg_send(internal_cmd_q, &opcode, BT_TIMEOUT_NONE);
		osif_msg_queue_peek(internal_cmd_q, &internal_cmd_cnt);
		/* BT_LOGA("internal send opcode 0x%04x, cnt %d\r\n", opcode, internal_cmd_cnt); */
	}

	hci_transport_send(*buf, buf + 1, len - 1, true);
	return true;
}
