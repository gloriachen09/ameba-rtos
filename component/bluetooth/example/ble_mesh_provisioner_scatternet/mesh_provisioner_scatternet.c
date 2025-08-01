
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gatts.h>
#include <rtk_service_config.h>
#include <rtk_simple_ble_service.h>
#include <rtk_dis.h>
#include <rtk_hrs.h>
#include <rtk_bas.h>
#include <rtk_ias.h>
#include <rtk_gls.h>
#include <rtk_hids_kb.h>
#include <rtk_long_uuid_service.h>
#include <rtk_bt_gattc.h>
#include <rtk_client_config.h>
#include <rtk_gcs_client.h>
#include <rtk_bas_client.h>
#include <rtk_gaps_client.h>
#include <rtk_simple_ble_client.h>

#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_config_client_model.h>
#include <rtk_bt_mesh_generic_onoff_model.h>
#include <rtk_bt_mesh_datatrans_model.h>
#include <rtk_bt_mesh_remote_prov_model.h>
#include <rtk_bt_mesh_light_model.h>
#include <rtk_bt_mesh_time_model.h>
#include <rtk_bt_mesh_scheduler_model.h>
#include <rtk_bt_mesh_scene_model.h>
#include <rtk_bt_mesh_generic_default_transition_time.h>
#include <rtk_bt_mesh_generic_model.h>
#include <rtk_bt_mesh_sensor_model.h>
#include <rtk_bt_mesh_health_model.h>
#include <rtk_bt_mesh_directed_forwarding_model.h>
#include <rtk_bt_mesh_subnet_bridge_model.h>
#include <rtk_bt_mesh_private_beacon_model.h>
#include <rtk_bt_mesh_device_firmware_update_model.h>
#include <bt_utils.h>

static uint8_t adv_data[] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x0c,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'M', 'E', 'S', 'H',
};
static rtk_bt_le_adv_param_t adv_param = {
	.interval_min = 352, //units of 0.625ms
	.interval_max = 352,
	.type = RTK_BT_LE_ADV_TYPE_IND,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static rtk_bt_le_security_param_t sec_param = {
	.io_cap = RTK_IO_CAP_NO_IN_NO_OUT,
	.oob_data_flag = 0,
	.bond_flag = 1,
	.mitm_flag = 0,
	.sec_pair_flag = 0,
	.use_fixed_key = 0,
	.fixed_key = 000000,
};

static void mesh_data_uart_dump(uint8_t *pbuffer, uint32_t len)
{
	BT_DUMPHEXA("0x", pbuffer, len, false);
}

static void app_server_disconnect(uint16_t conn_handle)
{
	simple_ble_srv_disconnect(conn_handle);
	bas_disconnect(conn_handle);
	hrs_disconnect(conn_handle);
	gls_disconnect(conn_handle);
}

static void app_server_deinit(void)
{
	simple_ble_srv_status_deinit();
	bas_status_deinit();
	hrs_status_deinit();
	gls_status_deinit();
}

static rtk_bt_evt_cb_ret_t ble_mesh_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;
	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_SCAN_START_IND: {
		/*  Do not report scan start event, because mesh stack would auto start and stop scan when enable extended adv and extended scan(RTK_BLE_MESH_BASED_ON_CODED_PHY)
		rtk_bt_le_scan_start_ind_t *scan_start_ind = (rtk_bt_le_scan_start_ind_t *)param;
		if (!scan_start_ind->err) {
		    BT_LOGA("[APP] Scan started, scan_type: %d\r\n", scan_start_ind->scan_type);
		} else {
		    BT_LOGE("[APP] Scan start failed(err: 0x%x)\r\n", scan_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,start,%d,%d\r\n", (scan_start_ind->err == 0) ? 0 : -1, scan_start_ind->scan_type);
		*/
		break;
	}
	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		/*  Do not report scan start event, because mesh stack would auto start and stop scan when enable extended adv and extended scan(RTK_BLE_MESH_BASED_ON_CODED_PHY)
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
		    BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
		} else {
		    BT_LOGE("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,stop,%d,0x%x\r\n", (scan_stop_ind->err == 0) ? 0 : -1, scan_stop_ind->stop_reason);
		*/
		break;
	}
	case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %d, len: %d \r\n",
				le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
				scan_res_ind->adv_report.len);
		break;
	}
	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		if (!conn_ind->err) {
			role = conn_ind->role ? "slave" : "master";
			BT_LOGA("[APP] Connected, handle: %d, role: %s, remote device: %s\r\n",
					conn_ind->conn_handle, role, le_addr);
			if (RTK_BT_LE_ROLE_SLAVE == conn_ind->role) {
#if !(defined(RTK_BLE_MESH_BASED_ON_CODED_PHY) && RTK_BLE_MESH_BASED_ON_CODED_PHY)
				// If not enable mesh based on coded PHY, stop mesh customer one shot adv timer, otherwise upstack may auto stop extended connectionalbe legacy ADV so do nothing
				rtk_bt_le_gap_stop_adv();
#endif
			}
#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
			/* gattc action */
			if (RTK_BT_LE_ROLE_MASTER == conn_ind->role &&
				RTK_BT_OK == general_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == bas_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == gaps_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == simple_ble_client_attach_conn(conn_ind->conn_handle)) {
				BT_LOGA("[APP] GATTC Profiles attach connection success, conn_handle: %d\r\n",
						conn_ind->conn_handle);
			}
#endif
			/* central action */
			if (RTK_BT_LE_ROLE_MASTER == conn_ind->role &&
				rtk_bt_le_sm_is_device_bonded(&conn_ind->peer_addr)) {
				BT_LOGA("[APP] Bonded device, start link encryption procedure\r\n");
				rtk_bt_le_sm_start_security(conn_ind->conn_handle);
			}
		} else {
			BT_LOGE("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
					conn_ind->err, le_addr);
		}
		BT_AT_PRINT("+BLEGAP:conn,%d,%d,%s\r\n", (conn_ind->err == 0) ? 0 : -1, (int)conn_ind->conn_handle, le_addr);
		break;
	}
	case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
		rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
		role = disconn_ind->role ? "slave" : "master";
		BT_LOGA("[APP] Disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
				disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		BT_AT_PRINT("+BLEGAP:disconn,0x%x,%d,%s,%s\r\n",
					disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		if (RTK_BT_LE_ROLE_SLAVE == disconn_ind->role) {
			// Start mesh custom ADV
			rtk_bt_le_gap_start_adv(&adv_param);
			app_server_disconnect(disconn_ind->conn_handle);
		}
		/* gattc action */
		if (RTK_BT_LE_ROLE_MASTER == disconn_ind->role &&
			RTK_BT_OK == general_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == bas_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == gaps_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == simple_ble_client_detach_conn(disconn_ind->conn_handle)) {
			BT_LOGA("[APP] GATTC Profiles detach connection success, conn_handle: %d\r\n",
					disconn_ind->conn_handle);
		}
		break;
	}
	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *conn_update_ind =
			(rtk_bt_le_conn_update_ind_t *)param;
		if (conn_update_ind->err) {
			BT_LOGE("[APP] Update conn param failed, conn_handle: %d, err: 0x%x\r\n",
					conn_update_ind->conn_handle, conn_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_update,%d,-1\r\n", conn_update_ind->conn_handle);
		} else {
			BT_LOGA("[APP] Conn param is updated, conn_handle: %d, conn_interval: 0x%x, "       \
					"conn_latency: 0x%x, supervision_timeout: 0x%x\r\n",
					conn_update_ind->conn_handle,
					conn_update_ind->conn_interval,
					conn_update_ind->conn_latency,
					conn_update_ind->supv_timeout);
			BT_AT_PRINT("+BLEGAP:conn_update,%d,0,0x%x,0x%x,0x%x\r\n",
						conn_update_ind->conn_handle,
						conn_update_ind->conn_interval,
						conn_update_ind->conn_latency,
						conn_update_ind->supv_timeout);
		}
		break;
	}
	case RTK_BT_LE_GAP_EVT_REMOTE_CONN_UPDATE_REQ_IND: { //BT api shall not be called here
		rtk_bt_le_remote_conn_update_req_ind_t *rmt_update_req =
			(rtk_bt_le_remote_conn_update_req_ind_t *)param;
		BT_LOGA("[APP] Remote device request a change in conn param, conn_handle: %d, "      \
				"conn_interval_max: 0x%x, conn_interval_min: 0x%x, conn_latency: 0x%x, "      \
				"timeout: 0x%x. The host stack accept it.\r\n",
				rmt_update_req->conn_handle,
				rmt_update_req->conn_interval_max,
				rmt_update_req->conn_interval_min,
				rmt_update_req->conn_latency,
				rmt_update_req->supv_timeout);
		return RTK_BT_EVT_CB_ACCEPT;
		break;
	}
	case RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND: {
		rtk_bt_le_phy_update_ind_t *phy_update_ind =
			(rtk_bt_le_phy_update_ind_t *)param;
		if (phy_update_ind->err) {
			BT_LOGE("[APP] Update PHY failed, conn_handle: %d, err: 0x%x\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,-1\r\n", phy_update_ind->conn_handle);
		} else {
			BT_LOGA("[APP] PHY is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->tx_phy,
					phy_update_ind->rx_phy);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,0,%d,%d\r\n",
						phy_update_ind->conn_handle,
						phy_update_ind->tx_phy,
						phy_update_ind->rx_phy);
		}
		break;
	}
	case RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND: {
		rtk_bt_le_data_len_change_ind_t *data_len_change =
			(rtk_bt_le_data_len_change_ind_t *)param;
		BT_LOGA("[APP] Data len is updated, conn_handle: %d, "       \
				"max_tx_octets: 0x%x, max_tx_time: 0x%x, "        \
				"max_rx_octets: 0x%x, max_rx_time: 0x%x\r\n",
				data_len_change->conn_handle,
				data_len_change->max_tx_octets,
				data_len_change->max_tx_time,
				data_len_change->max_rx_octets,
				data_len_change->max_rx_time);
		BT_AT_PRINT("+BLEGAP:conn_datalen,%d,0x%x,0x%x,0x%x,0x%x\r\n",
					data_len_change->conn_handle,
					data_len_change->max_tx_octets,
					data_len_change->max_tx_time,
					data_len_change->max_rx_octets,
					data_len_change->max_rx_time);
		break;
	}
	case RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND: {
		rtk_bt_le_bond_modify_ind_t *bond_mdf_ind =
			(rtk_bt_le_bond_modify_ind_t *)param;
		char ident_addr[30] = {0};
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->remote_addr), le_addr, sizeof(le_addr));
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->ident_addr), ident_addr, sizeof(ident_addr));
		BT_LOGA("[APP] Bond info modified, op: %d, addr: %s, ident_addr: %s\r\n",
				bond_mdf_ind->op, le_addr, ident_addr);
		BT_AT_PRINT("+BLEGAP:bond_modify,%d,%s,%s\r\n", bond_mdf_ind->op, le_addr, ident_addr);
		break;
	}
	case RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND: {
		rtk_bt_le_auth_complete_ind_t *auth_cplt_ind =
			(rtk_bt_le_auth_complete_ind_t *)param;
		BT_AT_PRINT("+BLEGAP:sec,%d,%d\r\n",
					auth_cplt_ind->conn_handle,
					(auth_cplt_ind->err == 0) ? 0 : -1);
		if (auth_cplt_ind->err) {
			BT_LOGE("[APP] Pairing failed(err: 0x%x), conn_handle: %d\r\n",
					auth_cplt_ind->err, auth_cplt_ind->conn_handle);
		} else {
			BT_LOGA("[APP] Pairing success, conn_handle: %d\r\n", auth_cplt_ind->conn_handle);
			BT_DUMPHEXA("[APP] long term key is 0x", auth_cplt_ind->dev_ltk, auth_cplt_ind->dev_ltk_length, true);
		}
		break;
	}
	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
		rtk_bt_le_auth_key_input_ind_t *key_input_ind =
			(rtk_bt_le_auth_key_input_ind_t *)param;
		BT_LOGA("[APP] Please input the auth passkey get from remote, conn_handle: %d\r\n",
				key_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_input,%d\r\n", key_input_ind->conn_handle);
		break;
	}
	case RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND: {
		rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind =
			(rtk_bt_le_auth_pair_cfm_ind_t *)param;
		BT_LOGA("[APP] Just work pairing need user to confirm, conn_handle: %d!\r\n",
				pair_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:pair_cfm,%d\r\n", pair_cfm_ind->conn_handle);
		rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
		uint16_t ret = 0;
		pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
		pair_cfm_param.confirm = 1;
		ret = rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] Just work pairing auto confirm succcess\r\n");
		}
		break;
	}
	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
		rtk_bt_le_auth_key_display_ind_t *key_dis_ind =
			(rtk_bt_le_auth_key_display_ind_t *)param;
		BT_LOGA("[APP] Auth passkey display: %d, conn_handle:%d\r\n",
				key_dis_ind->passkey,
				key_dis_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_display,%d,%d\r\n",
					(int)key_dis_ind->conn_handle,
					(int)key_dis_ind->passkey);
		break;
	}
	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
		rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind =
			(rtk_bt_le_auth_key_cfm_ind_t *)param;
		BT_LOGA("[APP] Auth passkey confirm: %d, conn_handle: %d. "  \
				"Please comfirm if the passkeys are equal!\r\n",
				key_cfm_ind->passkey,
				key_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_cfm,%d,%d\r\n",
					(int)key_cfm_ind->conn_handle,
					(int)key_cfm_ind->passkey);
		break;
	}
	case RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND: {
		rtk_bt_le_auth_oob_input_ind_t *oob_input_ind =
			(rtk_bt_le_auth_oob_input_ind_t *)param;
		BT_LOGA("[APP] Bond use oob key, conn_handle: %d. Please input the oob tk \r\n",
				oob_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:oobkey_input,%d\r\n", oob_input_ind->conn_handle);
		break;
	}
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_EVT_EXT_ADV_IND: {
		rtk_bt_le_ext_adv_ind_t *ext_adv_ind = (rtk_bt_le_ext_adv_ind_t *)param;
		if (!ext_adv_ind->err) {
			if (ext_adv_ind->is_start) {
				BT_LOGA("[APP] Ext ADV(%d) started\r\n", ext_adv_ind->adv_handle);
			} else {
				BT_LOGA("[APP] Ext ADV(%d) stopped: reason 0x%x \r\n", ext_adv_ind->adv_handle, ext_adv_ind->stop_reason);
			}
		} else {
			if (ext_adv_ind->is_start) {
				BT_LOGE("[APP] Ext ADV(%d) started failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			} else {
				BT_LOGE("[APP] Ext ADV(%d) stopped failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			}
		}
		BT_AT_PRINT("+BLEGAP:eadv,%s,%d,%d\r\n",
					ext_adv_ind->is_start ? "start" : "stop",
					(ext_adv_ind->err == 0) ? 0 : -1,
					ext_adv_ind->adv_handle);
		break;
	}
#endif

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

static uint16_t app_get_gatts_app_id(uint8_t event, void *data)
{
	uint16_t app_id = 0xFFFF;

	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *p_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
		app_id = p_reg_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND: {
		rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
		app_id = p_read_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_WRITE_IND: {
		rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
		app_id = p_write_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_CCCD_IND: {
		rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
		app_id = p_cccd_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND:
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ntf = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		app_id = p_ind_ntf->app_id;
		break;
	}
	default:
		BT_LOGE("[%s] Unknown event:%d\r\n", __func__, event);
		break;
	}
	return app_id;
}

static rtk_bt_evt_cb_ret_t ble_mesh_gatts_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	uint16_t app_id = 0xFFFF;

	if (RTK_BT_GATTS_EVT_MTU_EXCHANGE == event) {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			BT_LOGA("[APP] GATTS mtu exchange successfully, mtu_size: %d, conn_handle: %d \r\n",
					p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			BT_LOGE("[APP] GATTS mtu exchange fail \r\n");
		}
		return RTK_BT_EVT_CB_OK;
	}

	if (RTK_BT_GATTS_EVT_CLIENT_SUPPORTED_FEATURES == event) {
		rtk_bt_gatts_client_supported_features_ind_t *p_ind = (rtk_bt_gatts_client_supported_features_ind_t *)data;
		if (p_ind->features & RTK_BT_GATTS_CLIENT_SUPPORTED_FEATURES_EATT_BEARER_BIT) {
			BT_LOGA("[APP] Client Supported features is writed: conn_handle %d, features 0x%02x. Remote client supports EATT.\r\n",
					p_ind->conn_handle, p_ind->features);
		}
		return RTK_BT_EVT_CB_OK;
	}

	app_id = app_get_gatts_app_id(event, data);
	switch (app_id) {
	case SIMPLE_BLE_SRV_ID:
		simple_ble_service_callback(event, data);
		break;
	case DEVICE_INFO_SRV_ID:
		device_info_srv_callback(event, data);
		break;
	case HEART_RATE_SRV_ID:
		heart_rate_srv_callback(event, data);
		break;
	case BATTERY_SRV_ID:
		battery_service_callback(event, data);
		break;
	case IMMEDIATE_ALERT_SRV_ID:
		immediate_alert_srv_callback(event, data);
		break;
	case GLUCOSE_SRV_ID:
		glucose_srv_callback(event, data);
		break;
	case HID_SRV_ID:
		hid_srv_callback(event, data);
		break;
	case LONG_UUID_SRV_ID:
		long_uuid_service_callback(event, data);
		break;
	default:
		BT_LOGE("[%s] Unknown app_id:0x%x\r\n", __func__, app_id);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static uint16_t app_get_gattc_profile_id(uint8_t event, void *data)
{
	uint16_t profile_id = 0xFFFF;

	switch (event) {
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND: {
		rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
		profile_id = disc_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_READ_RESULT_IND: {
		rtk_bt_gattc_read_ind_t *read_res = (rtk_bt_gattc_read_ind_t *)data;
		profile_id = read_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND: {
		rtk_bt_gattc_write_ind_t *write_res = (rtk_bt_gattc_write_ind_t *)data;
		profile_id = write_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_NOTIFY_IND:
	case RTK_BT_GATTC_EVT_INDICATE_IND: {
		rtk_bt_gattc_cccd_value_ind_t *cccd_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
		profile_id = cccd_ind->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_CCCD_ENABLE_IND:
	case RTK_BT_GATTC_EVT_CCCD_DISABLE_IND: {
		rtk_bt_gattc_cccd_update_ind_t *cccd_update = (rtk_bt_gattc_cccd_update_ind_t *)data;
		profile_id = cccd_update->profile_id;
		break;
	}
	default:
		BT_LOGE("[%s] Unknown event:%d\r\n", __func__, event);
		break;
	}

	return profile_id;
}

static rtk_bt_evt_cb_ret_t ble_mesh_gattc_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	uint16_t profile_id = 0xFFFF;

	if (RTK_BT_GATTC_EVT_MTU_EXCHANGE == event) {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			BT_LOGA("[APP] GATTC mtu exchange success, mtu_size: %d, conn_handle: %d \r\n",
					p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			BT_LOGE("[APP] GATTC mtu exchange fail \r\n");
		}
		return RTK_BT_EVT_CB_OK;
	}

	profile_id = app_get_gattc_profile_id(event, data);
	switch (profile_id) {
	case GCS_CLIENT_PROFILE_ID:
		general_client_app_callback(event, data);
		break;
	case GAPS_CLIENT_PROFILE_ID:
		gaps_client_app_callback(event, data);
		break;
	case BAS_CLIENT_PROFILE_ID:
		bas_client_app_callback(event, data);
		break;
	case SIMPLE_BLE_CLIENT_PROFILE_ID:
		simple_ble_client_app_callback(event, data);
		break;
	default:
		BT_LOGE("[%s] Unknown profile_id:0x%x\r\n", __func__, profile_id);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t ble_mesh_stack_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	rtk_bt_evt_cb_ret_t ret = RTK_BT_EVT_CB_OK;
	switch (evt_code) {
	case RTK_BT_MESH_STACK_EVT_SCAN_SWITCH: {
		rtk_bt_mesh_stack_evt_scan_switch_t *scan_switch;
		scan_switch = (rtk_bt_mesh_stack_evt_scan_switch_t *)param;
		if (scan_switch->scan_enable) {
			BT_LOGA("[APP] Ble mesh scan enable SUCCESS\r\n\r\n");
		} else {
			BT_LOGA("[APP] Ble mesh scan disable SUCCESS\r\n\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_START_ADV: {
		rtk_bt_mesh_stack_evt_start_adv_t *start_adv;
		start_adv = (rtk_bt_mesh_stack_evt_start_adv_t *)param;
		if (RTK_BT_MESH_STACK_API_SUCCESS == start_adv->result) {
			BT_LOGA("[APP] Ble mesh start ADV SUCCESS\r\n");
		} else {
			BT_LOGE("[APP] Ble mesh start ADV fail!\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_STOP_ADV: {
		rtk_bt_mesh_stack_evt_stop_adv_t *stop_adv;
		stop_adv = (rtk_bt_mesh_stack_evt_stop_adv_t *)param;
		if (RTK_BT_MESH_STACK_API_SUCCESS == stop_adv->result) {
			BT_LOGA("[APP] Ble mesh stop ADV SUCCESS\r\n");
		} else {
			BT_LOGE("[APP] Ble mesh stop ADV fail!\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_UDB_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_udb_t *udb_info;
		udb_info = (rtk_bt_mesh_stack_evt_dev_info_udb_t *)param;
		BT_LOGA("[APP] ");
		if (0xff != udb_info->dev_info.bt_addr_type) {
			// Only for RTK mesh stack, report bt address and type information(zephyr mesh stack can not report)
			BT_LOGA("bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", udb_info->dev_info.bt_addr[5], udb_info->dev_info.bt_addr[4],
					udb_info->dev_info.bt_addr[3], udb_info->dev_info.bt_addr[2], udb_info->dev_info.bt_addr[1], udb_info->dev_info.bt_addr[0], udb_info->dev_info.bt_addr_type,
					udb_info->dev_info.rssi);
		}
		BT_LOGA("udb=");
		mesh_data_uart_dump(udb_info->dev_uuid, 16);
		BT_AT_PRINT("+BLEMESHSTACK:dev_info,0x%02x%02x%02x%02x%02x%02x,%d,%d,udb,",
					udb_info->dev_info.bt_addr[5], udb_info->dev_info.bt_addr[4], udb_info->dev_info.bt_addr[3],
					udb_info->dev_info.bt_addr[2], udb_info->dev_info.bt_addr[1], udb_info->dev_info.bt_addr[0],
					udb_info->dev_info.bt_addr_type, udb_info->dev_info.rssi);
		BT_AT_DUMP_HEXN(udb_info->dev_uuid, 16);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_SNB_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_snb_t *snb_info;
		snb_info = (rtk_bt_mesh_stack_evt_dev_info_snb_t *)param;
		BT_LOGA("[APP] ");
		if (0xff != snb_info->dev_info.bt_addr_type) {
			// Only for RTK mesh stack, report bt address and type information(zephyr mesh stack can not report)
			BT_LOGA("bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", snb_info->dev_info.bt_addr[5], snb_info->dev_info.bt_addr[4],
					snb_info->dev_info.bt_addr[3], snb_info->dev_info.bt_addr[2], snb_info->dev_info.bt_addr[1], snb_info->dev_info.bt_addr[0], snb_info->dev_info.bt_addr_type,
					snb_info->dev_info.rssi);
		}
		BT_LOGA("snb=");
		mesh_data_uart_dump(snb_info->net_id, 8);
		BT_AT_PRINT("+BLEMESHSTACK:dev_info,0x%02x%02x%02x%02x%02x%02x,%d,%d,snb,",
					snb_info->dev_info.bt_addr[5], snb_info->dev_info.bt_addr[4], snb_info->dev_info.bt_addr[3],
					snb_info->dev_info.bt_addr[2], snb_info->dev_info.bt_addr[1], snb_info->dev_info.bt_addr[0],
					snb_info->dev_info.bt_addr_type, snb_info->dev_info.rssi);
		BT_AT_DUMP_HEXN(snb_info->net_id, 8);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROV_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_provision_adv_t *prov_info;
		prov_info = (rtk_bt_mesh_stack_evt_dev_info_provision_adv_t *)param;
		BT_LOGA("[APP] ");
		if (0xff != prov_info->dev_info.bt_addr_type) {
			// Only for RTK mesh stack, report bt address and type information(zephyr mesh stack can not report)
			BT_LOGA("bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", prov_info->dev_info.bt_addr[5], prov_info->dev_info.bt_addr[4],
					prov_info->dev_info.bt_addr[3], prov_info->dev_info.bt_addr[2], prov_info->dev_info.bt_addr[1], prov_info->dev_info.bt_addr[0],
					prov_info->dev_info.bt_addr_type, prov_info->dev_info.rssi);
		}
		BT_LOGA("prov=");
		mesh_data_uart_dump(prov_info->dev_uuid, 16);
		BT_AT_PRINT("+BLEMESHSTACK:dev_info,0x%02x%02x%02x%02x%02x%02x,%d,%d,prov,",
					prov_info->dev_info.bt_addr[5], prov_info->dev_info.bt_addr[4], prov_info->dev_info.bt_addr[3],
					prov_info->dev_info.bt_addr[2], prov_info->dev_info.bt_addr[1], prov_info->dev_info.bt_addr[0],
					prov_info->dev_info.bt_addr_type, prov_info->dev_info.rssi);
		BT_AT_DUMP_HEXN(prov_info->dev_uuid, 16);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROXY_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t *proxy_info;
		proxy_info = (rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t *)param;
		switch (proxy_info->proxy.type) {
		case RTK_BT_MESH_PROXY_ADV_TYPE_NET_ID:
			BT_LOGA("proxy net id=");
			break;
		case RTK_BT_MESH_PROXY_ADV_TYPE_NODE_IDENTITY:
			BT_LOGA("proxy node id=");
			break;
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON) && BT_MESH_ENABLE_PRIVATE_BEACON
		case RTK_BT_MESH_PROXY_ADV_TYPE_PRIVATE_NET_ID:
			BT_LOGA("proxy private net id=");
			break;
		case RTK_BT_MESH_PROXY_ADV_TYPE_PRIVATE_NODE_IDENTITY:
			BT_LOGA("proxy private node id=");
			break;
#endif
		}
		mesh_data_uart_dump((uint8_t *)&proxy_info->proxy, proxy_info->len);
		BT_AT_PRINT("+BLEMESHSTACK:dev_info,0x%02x%02x%02x%02x%02x%02x,%d,%d,proxy,%d,",
					proxy_info->dev_info.bt_addr[5], proxy_info->dev_info.bt_addr[4], proxy_info->dev_info.bt_addr[3],
					proxy_info->dev_info.bt_addr[2], proxy_info->dev_info.bt_addr[1], proxy_info->dev_info.bt_addr[0],
					proxy_info->dev_info.bt_addr_type, proxy_info->dev_info.rssi, proxy_info->proxy.type);
		BT_AT_DUMP_HEXN((uint8_t *)&proxy_info->proxy, proxy_info->len);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE: {
		rtk_bt_mesh_prov_generic_cb_type_t *pb_adv_link_state;
		pb_adv_link_state = (rtk_bt_mesh_prov_generic_cb_type_t *)param;
		BT_AT_PRINT("+BLEMESHSTACK:pbadv_state,%d\r\n", *pb_adv_link_state);
		switch (*pb_adv_link_state) {
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_OPENED:
			BT_LOGA("[APP] PB-ADV Link Opened!\r\n");
			break;
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_OPEN_FAILED:
			BT_LOGA("[APP] PB-ADV Link Open Failed!\r\n");
			break;
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_CLOSED:
			BT_LOGA("[APP] PB-ADV Link Closed!\r\n");
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROV_COMPLETE: {
		rtk_bt_mesh_stack_evt_prov_complete_t *prov_complete;
		prov_complete = (rtk_bt_mesh_stack_evt_prov_complete_t *)param;
		if (prov_complete->dkri_flag) {
			BT_AT_PRINT("+BLEMESHSTACK:dkri,%d\r\n", prov_complete->dkri);
			switch (prov_complete->dkri) {
			case RTK_BT_MESH_RMT_PROV_DKRI_DEV_KEY_REFRESH:
				BT_LOGA("[APP] Refresh device key!\r\n");
				break;
			case RTK_BT_MESH_RMT_PROV_DKRI_NODE_ADDR_REFRESH:
				BT_LOGA("[APP] Refresh node address to 0x%04x!\r\n", prov_complete->unicast_addr);
				BT_AT_PRINT("+BLEMESHSTACK:dkri,1,%d\r\n", prov_complete->unicast_address);
				break;
			case RTK_BT_MESH_RMT_PROV_DKRI_NODE_COMPO_REFRESH:
				BT_LOGA("[APP] Refresh node composition data!\r\n");
				break;
			default:
				BT_LOGE("[APP] Unknown dkri procedure %d done!\r\n", prov_complete->dkri);
				break;
			}
		} else {
			BT_LOGA("[APP] Provisioning complete,unicast address:0x%x\r\n", prov_complete->unicast_addr);
			BT_AT_PRINT("+BLEMESHSTACK:prov,0,0x%x\r\n", prov_complete->unicast_addr);
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROV_FAIL: {
		rtk_bt_mesh_stack_evt_prov_fail_t *prov_fail;
		prov_fail = (rtk_bt_mesh_stack_evt_prov_fail_t *)param;
		BT_LOGE("[APP] Provisioning fail,reason:%d\r\n", prov_fail->fail_reason);
		BT_AT_PRINT("+BLEMESHSTACK:prov,-1,%d\r\n", prov_fail->fail_reason);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROV_SERVICE_DIS_RESULT: {
		rtk_bt_mesh_stack_evt_prov_dis_t *dis_res;
		dis_res = (rtk_bt_mesh_stack_evt_prov_dis_t *)param;
		BT_AT_PRINT("+BLEMESHSTACK:prov_dis,%d\r\n", dis_res->dis_result);
		switch (dis_res->dis_result) {
		case RTK_BT_MESH_PROV_DISC_DONE:
			BT_LOGA("[APP] Prov service discovery done!\r\n");
			break;
		case RTK_BT_MESH_PROV_DISC_FAIL:
			BT_LOGE("[APP] Prov service discovery fail!\r\n");
			break;
		case RTK_BT_MESH_PROV_DISC_NOT_FOUND:
			BT_LOGE("[APP] Prov service not found!\r\n");
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROXY_SERVICE_DIS_RESULT: {
		rtk_bt_mesh_stack_evt_proxy_dis_t *dis_res;
		dis_res = (rtk_bt_mesh_stack_evt_proxy_dis_t *)param;
		BT_AT_PRINT("+BLEMESHSTACK:proxy_dis,%d\r\n", dis_res->dis_result);
		switch (dis_res->dis_result) {
		case RTK_BT_MESH_PROXY_DISC_DONE:
			BT_LOGA("[APP] Proxy service discovery done!\r\n");
			break;
		case RTK_BT_MESH_PROXY_DISC_FAIL:
			BT_LOGE("[APP] Proxy service discovery fail!\r\n");
			break;
		case RTK_BT_MESH_PROXY_DISC_NOT_FOUND:
			BT_LOGE("[APP] Proxy service not found!\r\n");
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_FN_CB: {
		rtk_bt_mesh_stack_evt_fn_t *fn_cb = (rtk_bt_mesh_stack_evt_fn_t *)param;
		switch (fn_cb->cb_type) {
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISHING:
			BT_LOGA("[APP] Establishing friendship with LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_FAIL_NO_POLL:
			BT_LOGA("[APP] Not receive poll LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_SUCCESS:
			BT_LOGA("[APP] Established friendship with LPN: 0x%04x success\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_FRND_LOST:
			BT_LOGA("[APP] Lost friendship with LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		default:
			BT_LOGE("[%s] Unknown cb type:%d\r\n", __func__, fn_cb->cb_type);
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_CAPABILITY_NOT_SUPPORT_OF_DEVICE: {
		rtk_bt_mesh_stack_prov_capabilities_t *prov_capa = (rtk_bt_mesh_stack_prov_capabilities_t *)param;
		BT_LOGA("[APP] The prov capability of remote device is not strong:");
		BT_LOGA(" Element num:%d, Algorithm:0x%02x, Public Key Type:0x%02x, Static OOB Type:0x%02x, Output OOB Size:%d, Output OOB Action:0x%02x, Input OOB Size:%d, Input OOB Actions:0x%02x\r\n",
				\
				prov_capa->element_num, prov_capa->algorithm, prov_capa->public_key, prov_capa->static_oob, \
				prov_capa->output_oob_size, prov_capa->output_oob_action, prov_capa->input_oob_size, prov_capa->input_oob_action);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_STATIC_OOB: {
		rtk_bt_mesh_stack_set_auth_value_for_static_oob *static_oob;
		static_oob = (rtk_bt_mesh_stack_set_auth_value_for_static_oob *)param;
		if (static_oob->status) {
			BT_DUMPA("[APP] Set auth value for static oob success:\r\n", static_oob->data, 16);
		} else {
			BT_LOGE("[APP] Set auth value for static oob fail\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_INPUT_OOB: {
		rtk_bt_mesh_stack_set_auth_value_for_oob_data *output_oob;
		output_oob = (rtk_bt_mesh_stack_set_auth_value_for_oob_data *)param;
		if (output_oob->status) {
			BT_LOGA("[APP] Set auth value for output oob success, output random value:%d\r\n", (int)output_oob->random);
		} else {
			BT_LOGE("[APP] Set auth value for output oob fail\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_NOTIFY_FOR_OUTPUT_OOB_VALUE: {
		BT_LOGA("[APP] Please input the random value display on remote unprovisioned device\r\n");
		break;
	}
	case RTK_BT_MESH_STACK_EVT_LIST_INFO: {
		// Indicate event data struct : type(1 byte) + length(1 byte) + value(n bytes) + type +len + val ......
		uint8_t *p_data = (uint8_t *)param;
		uint32_t offset = 0, type, data_len;
		while (offset < len) {
			type = *(p_data + offset);
			offset++;
			data_len = *(p_data + offset);
			offset++;
			// In case of IPC data align , which will cause the event report len larger than actual data len
			if (offset + data_len > len) {
				break;
			}
			switch (type) {
			case RTK_BT_MESH_STACK_USER_LIST_MESH_STATE:
				BT_LOGA("Mesh State:\t%d\r\n", *(p_data + offset));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d\r\n", type, *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_UUID:
				BT_LOGA("Device UUID:\t");
				mesh_data_uart_dump(p_data + offset, data_len);
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,", type);
				BT_AT_DUMP_HEXN(p_data + offset, data_len);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_ADDR:
				BT_LOGA("BTAddr:\t\t0x%02x%02x%02x%02x%02x%02x\r\n",
						*(p_data + offset + 5), *(p_data + offset + 4), *(p_data + offset + 3), *(p_data + offset + 2), *(p_data + offset + 1), *(p_data + offset));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,0x%02x%02x%02x%02x%02x%02x\r\n",
							type, *(p_data + offset + 5), *(p_data + offset + 4), *(p_data + offset + 3),
							*(p_data + offset + 2), *(p_data + offset + 1), *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_KEY:
				BT_LOGA("DevKey:\t\t%d-0x%04x-%d-", *(p_data + offset), LE_TO_U16(p_data + offset + 1), *(p_data + offset + 3));
				mesh_data_uart_dump(p_data + offset + 4, 16);
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-0x%04x-%d-",
							type, *(p_data + offset), LE_TO_U16(p_data + offset + 1), *(p_data + offset + 3));
				BT_AT_DUMP_HEXN(p_data + offset + 4, 16);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_NET_KEY: {
				uint8_t net_key_type = *(p_data + offset);
				switch (net_key_type) {
				case RTK_BT_MESH_NET_KEY_MASTER:
					BT_LOGA("NetKey:\t\t");
					break;
				case RTK_BT_MESH_NET_KEY_FN:
					BT_LOGA("NetKey-FN:\t");
					break;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
				case RTK_BT_MESH_NET_KEY_DF:
					BT_LOGA("NetKey-DF:\t");
					break;
#endif
				default:
					break;
				}
				BT_LOGA("%d-0x%04x-%d-%d-%d\r\n", *(p_data + offset + 1), LE_TO_U16(p_data + offset + 2), \
						* (p_data + offset + 4), *(p_data + offset + 5), *(p_data + offset + 6));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d,%d-0x%04x-%d-%d-%d-",
							type, net_key_type, *(p_data + offset + 1), LE_TO_U16(p_data + offset + 2), \
							* (p_data + offset + 4), *(p_data + offset + 5), *(p_data + offset + 6));
				if (net_key_type == 1) {
					BT_LOGA("\t\t");
					mesh_data_uart_dump(p_data + offset + 7, 16);
					BT_AT_DUMP_HEXN(p_data + offset + 7, 16);
				}
			}
			break;
			case RTK_BT_MESH_STACK_USER_LIST_APP_KEY:
				BT_LOGA("AppKey:\t\t%d-0x%04x-%d-%d-%d\r\n\t\t", *(p_data + offset), LE_TO_U16(p_data + offset + 1), \
						* (p_data + offset + 3), *(p_data + offset + 4), LE_TO_U16(p_data + offset + 5));
				mesh_data_uart_dump(p_data + offset + 7, 16);
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-0x%04x-%d-%d-%d-",
							type, *(p_data + offset), LE_TO_U16(p_data + offset + 1),
							*(p_data + offset + 3), *(p_data + offset + 4), LE_TO_U16(p_data + offset + 5));
				BT_AT_DUMP_HEXN(p_data + offset + 7, 16);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_NORMAL_VALS:
				BT_LOGA("IVindex:\t%d-0x%x\r\n", *(p_data + offset), LE_TO_U32(p_data + offset + 1));
				BT_LOGA("Seq:\t\t0x%06x\r\n", LE_TO_U32(p_data + offset + 5));
				BT_LOGA("NodeAddr:\t0x%04x-%d-%d\r\n", LE_TO_U16(p_data + offset + 9),
						LE_TO_U32(p_data + offset + 11), *(p_data + offset + 15));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-0x%x,0x%06x,0x%04x-%d-%d\r\n",
							type, *(p_data + offset), LE_TO_U32(p_data + offset + 1),
							LE_TO_U32(p_data + offset + 5), LE_TO_U16(p_data + offset + 9),
							LE_TO_U32(p_data + offset + 11), *(p_data + offset + 15));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_ELEMENT_INFO:
				BT_LOGA("Element:\t%d-%d", *(p_data + offset), LE_TO_U32(p_data + offset + 1));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-%d",
							type, *(p_data + offset), LE_TO_U32(p_data + offset + 1));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_INFO:
				BT_LOGA("\r\nModel:\t\t%d-%d-0x%08x", *(p_data + offset), *(p_data + offset + 1), LE_TO_U32(p_data + offset + 2));
				BT_AT_PRINT("\r\n+BLEMESHSTACK:list,%d,%d-%d-0x%08x",
							type, *(p_data + offset), *(p_data + offset + 1), LE_TO_U32(p_data + offset + 2));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_APP_KEY:
				BT_LOGA("(app key:%d)", *(p_data + offset));
				BT_AT_PRINT(",%d,%d", type, *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_PUB_INFO:
				BT_LOGA("(pub to:0x%04x-%d-%d", LE_TO_U16(p_data + offset), *(p_data + offset + 2),
						LE_TO_U16(p_data + offset + 3));
				BT_AT_PRINT(",%d,0x%04x-%d-%d", type, LE_TO_U16(p_data + offset), *(p_data + offset + 2),
							LE_TO_U16(p_data + offset + 3));
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
				BT_LOGA("-%d", *(p_data + offset + 5));
				BT_AT_PRINT("-%d", *(p_data + offset + 5));
#endif
				BT_LOGA(")");
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_SUB_INFO: {
				uint16_t sub_addr_num = data_len / 2;
				uint16_t temp_offset = offset;
				BT_LOGA("(sub to:");
				BT_AT_PRINT(",%d,", type);
				for (uint16_t i = 0; i < sub_addr_num; i++) {
					BT_LOGA("0x%04x", LE_TO_U16(p_data + temp_offset));
					BT_AT_PRINT("0x%04x", LE_TO_U16(p_data + temp_offset));
					temp_offset += 2;
					if (i == sub_addr_num - 1) {
						BT_LOGA(")");
					} else {
						BT_LOGA("-");
						BT_AT_PRINT("-");
					}
				}
				break;
			}
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
			case RTK_BT_MESH_STACK_USER_LIST_DF_PATH_INFO: {
				BT_LOGA("\r\nDF-Path:\t%d-%d-%d-%d-%d-%d-0x%04x-0x%04x\r\n\t\t0x%04x(%d)",
						LE_TO_U16(p_data + offset), LE_TO_U16(p_data + offset + 2),
						*(p_data + offset + 4), *(p_data + offset + 5),
						*(p_data + offset + 6), *(p_data + offset + 7),
						LE_TO_U16(p_data + offset + 8), LE_TO_U16(p_data + offset + 10),
						LE_TO_U16(p_data + offset + 12), *(p_data + offset + 14));
				BT_AT_PRINT("\r\n+BLEMESHSTACK:list,%d,%d-%d-%d-0x%04x,%d",
							type, LE_TO_U16(p_data + offset), LE_TO_U16(p_data + offset + 2),
							*(p_data + offset + 4), *(p_data + offset + 5),
							*(p_data + offset + 6), *(p_data + offset + 7),
							LE_TO_U16(p_data + offset + 8), LE_TO_U16(p_data + offset + 10),
							LE_TO_U16(p_data + offset + 12), *(p_data + offset + 14));
				uint8_t origin_dependent_count = *(p_data + offset + 15);
				for (int i = 0; i < origin_dependent_count; i++) {
					BT_LOGA("-<0x%04x(%d)>", LE_TO_U16(p_data + offset + 16 + 3 * i), *(p_data + offset + 18 + 3 * i));
					BT_AT_PRINT(",0x%04x,%d", LE_TO_U16(p_data + offset + 16 + 3 * i), *(p_data + offset + 18 + 3 * i));
				}
				BT_LOGA("\r\n\t\t----->\r\n\t\t0x%04x(%d)", LE_TO_U16(p_data + offset + 16 + 3 * origin_dependent_count),
						*(p_data + offset + 18 + 3 * origin_dependent_count));
				BT_AT_PRINT("\r\n0x%04x,%d", LE_TO_U16(p_data + offset + 16 + 3 * origin_dependent_count),
							*(p_data + offset + 18 + 3 * origin_dependent_count));
				uint8_t target_dependent_count = *(p_data + offset + 19 + 3 * origin_dependent_count);
				for (int i = 0; i < target_dependent_count; i++) {
					BT_LOGA("-<0x%04x(%d)>", LE_TO_U16(p_data + offset + 20 + 3 * origin_dependent_count + 3 * i),
							*(p_data + offset + 22 + 3 * origin_dependent_count + 3 * i));
					BT_AT_PRINT(",0x%04x,%d", LE_TO_U16(p_data + offset + 20 + 3 * origin_dependent_count + 3 * i),
								*(p_data + offset + 22 + 3 * origin_dependent_count + 3 * i));
				}
				break;
			}
#endif
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE) && BT_MESH_ENABLE_SUBNET_BRIDGE
			case RTK_BT_MESH_STACK_USER_LIST_SUBNET_BRIDGE_INFO: {
				BT_LOGA("\r\nBridge:\t\t%d-%d-0x%04x-0x%04x-0x%04x-0x%04x\r\n", LE_TO_U16(p_data + offset), *(p_data + offset + 2),
						LE_TO_U16(p_data + offset + 3), LE_TO_U16(p_data + offset + 5), LE_TO_U16(p_data + offset + 7), LE_TO_U16(p_data + offset + 9));
				BT_AT_PRINT("\r\n+BLEMESHSTACK:list,%d,%d-%d-0x%04x-0x%04x-0x%04x-0x%04x\r\n", type, LE_TO_U16(p_data + offset), *(p_data + offset + 2),
							LE_TO_U16(p_data + offset + 3), LE_TO_U16(p_data + offset + 5), LE_TO_U16(p_data + offset + 7), LE_TO_U16(p_data + offset + 9));
				break;
			}
#endif
			default:
				BT_LOGE("[%s] Unknown data type %d for RTK_BT_MESH_STACK_EVT_LIST_INFO\r\n", __func__, (int)type);
				return RTK_BT_EVT_CB_OK;
				break;
			}
			offset += data_len;
		}
		BT_LOGA("\r\n\r\n");
		BT_AT_PRINT("\r\n");
		if (!len) {
			BT_LOGE("[APP] Print user cmd list fail, len==0\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_RETRANS_PARAM_SETTING_RESULT: {
		rtk_bt_mesh_stack_set_retrans_param_t *result;
		result = (rtk_bt_mesh_stack_set_retrans_param_t *)param;
		BT_LOGA("Set node net trans count %d, relay retrans count %d, ttl %d, trans retrans count %d, Success!\r\n",
				result->net_retrans_count, result->relay_retrans_count, result->ttl, result->trans_retrans_count);
		BT_AT_PRINT("+BLEMESHSTACK:retran_set,%d,%d,%d,%d\r\n",
					result->net_retrans_count, result->relay_retrans_count,
					result->ttl, result->trans_retrans_count);
		break;
	}
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
	case RTK_BT_MESH_STACK_EVT_DF_CB: {
		rtk_bt_mesh_stack_evt_df_t *df;
		df = (rtk_bt_mesh_stack_evt_df_t *)param;
		switch (df->type) {
		case RTK_BT_MESH_MSG_DF_PATH_ACTION: {
			char *state_str[] = {"Discovering", "Discovery failed", "Established", "Path released",
								 "New lane discovering", "New lane established", "New lane failed"
								};
			BT_LOGA("%s, master key index %d\r\nsrc 0x%04x(%d) [0x%04x(%d)] --> dst 0x%04x(%d) [0x%04x(%d)]\r\nforwarding num %d\r\n",
					state_str[df->path_action.action_type], df->path_action.master_key_index,
					df->path_action.path_src, df->path_action.path_src_sec_elem_num,
					df->path_action.dp_src, df->path_action.dp_src_sec_elem_num,
					df->path_action.path_dst, df->path_action.path_dst_sec_elem_num,
					df->path_action.dp_dst, df->path_action.dp_dst_sec_elem_num,
					df->path_action.forwarding_num);
			BT_AT_PRINT("+BLEMESHSTACK:df_state,%s, master key index %d\r\nsrc 0x%04x(%d) [0x%04x(%d)] --> dst 0x%04x(%d) [0x%04x(%d)]\r\nforwarding num %d\r\n",
						df->path_action.action_type, df->path_action.master_key_index,
						df->path_action.path_src, df->path_action.path_src_sec_elem_num,
						df->path_action.dp_src, df->path_action.dp_src_sec_elem_num,
						df->path_action.path_dst, df->path_action.path_dst_sec_elem_num,
						df->path_action.dp_dst, df->path_action.dp_dst_sec_elem_num,
						df->path_action.forwarding_num);
		}
		break;
		default:
			break;
		}
		break;
	}
#endif
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return ret;
}

static rtk_bt_evt_cb_ret_t ble_mesh_config_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_CONFIG_MODEL_APP_KEY_STAT: {
		rtk_bt_mesh_cfg_app_key_stat_t *add_app_key_state;
		add_app_key_state = (rtk_bt_mesh_cfg_app_key_stat_t *)param;
		if (add_app_key_state->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Config client model receive Config AppKey Status message with status SUCCESS\r\n");
		} else {
			BT_LOGE("[APP] Config client model receive Config AppKey Status message with status:0x%x\r\n", add_app_key_state->stat);
		}
		BT_AT_PRINT("+BLEMESHCONFIG:app_key_state,0x%x\r\n", add_app_key_state->stat);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_STAT: {
		rtk_bt_mesh_cfg_model_app_stat_t *model_app_bind_state;
		model_app_bind_state = (rtk_bt_mesh_cfg_model_app_stat_t *)param;
		if (model_app_bind_state->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Config client model receive Config Model App Status message with status SUCCESS\r\n");
		} else {
			BT_LOGE("[APP] Config client model receive Config Model App Status message with status:0x%x\r\n", model_app_bind_state->stat);
		}
		BT_AT_PRINT("+BLEMESHCONFIG:model_app_state,0x%x\r\n", model_app_bind_state->stat);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_STAT: {
		rtk_bt_mesh_cfg_model_sub_stat_t *model_sub_add_state;
		model_sub_add_state = (rtk_bt_mesh_cfg_model_sub_stat_t *)param;
		if (model_sub_add_state->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Config client model receive Config Model Subscription Status message with status SUCCESS\r\n");
		} else {
			BT_LOGE("[APP] Config client model receive Config Model Subscription Status message with status:0x%x\r\n", model_sub_add_state->stat);
		}
		BT_AT_PRINT("+BLEMESHCONFIG:model_sub_state,0x%x\r\n", model_sub_add_state->stat);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_STAT: {
		rtk_bt_mesh_cfg_default_ttl_stat_t *ttl_stat;
		ttl_stat = (rtk_bt_mesh_cfg_default_ttl_stat_t *)param;
		BT_LOGA("[APP] Config client model receive message:default TTL:%d\r\n", ttl_stat->ttl);
		BT_AT_PRINT("+BLEMESHCONFIG:ttl,%d\r\n", ttl_stat->ttl);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_RELAY_STAT: {
		rtk_bt_mesh_cfg_relay_stat_t *relay_stat;
		relay_stat = (rtk_bt_mesh_cfg_relay_stat_t *)param;
		BT_LOGA("[APP] Config client model receive message,Relay State: %d, Count = %d Step = %d\r\n", relay_stat->state, relay_stat->count, relay_stat->steps);
		BT_AT_PRINT("+BLEMESHCONFIG:relay_state,%d,%d,%d\r\n",
					relay_stat->state, relay_stat->count, relay_stat->steps);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_STAT: {
		rtk_bt_mesh_cfg_net_transmit_stat_t *net_transmit;
		net_transmit = (rtk_bt_mesh_cfg_net_transmit_stat_t *)param;
		BT_LOGA("[APP] Config client model receive message,Net transmit state: Count = %d Step = %d\r\n", net_transmit->count, net_transmit->steps);
		BT_AT_PRINT("+BLEMESHCONFIG:net_trans_state,%d,%d\r\n",
					net_transmit->count, net_transmit->steps);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NODE_RESET_STAT: {
		rtk_bt_mesh_config_client_model_node_reset_stat_t *node_reset_stat;
		node_reset_stat = (rtk_bt_mesh_config_client_model_node_reset_stat_t *)param;
		BT_LOGA("[APP] Config client model receive message:Node 0x%x is reset SUCCESS\r\n", node_reset_stat->src);
		BT_AT_PRINT("+BLEMESHCONFIG:reset,0,0x%x\r\n", node_reset_stat->src);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_BEACON_STAT: {
		rtk_bt_mesh_cfg_beacon_stat_t *beacon_stat;
		beacon_stat = (rtk_bt_mesh_cfg_beacon_stat_t *)param;
		BT_LOGA("[APP] beacon state: %d\r\n", beacon_stat->state);
		BT_AT_PRINT("+BLEMESHCONFIG:beacon_state,%d\r\n", beacon_stat->state);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_STAT: {
		BT_LOGA("[APP] Config client model receive message:COMPO DATA STAT SUCCESS\r\n");
		uint8_t *p_data = (uint8_t *)param;
		if (p_data[5] == 1) {
			uint16_t msg_len = LE_TO_U16(p_data + 1);
			uint16_t src = LE_TO_U16(p_data + 3);
			//  uint32_t length = LE_TO_U32(p_data + 6);// callback to app msg data length
			uint8_t element_index = 0;
			uint8_t *pdata = p_data + 10;
			uint8_t *pend = p_data + 10 + msg_len - 2;//2 is head length
			BT_LOGA("cdp0 parsed: src=0x%04x cid=0x%04x pid=0x%04x vid=0x%04x rpl=%d features=0x%04x\r\n",
					src, LE_TO_U16(pdata), LE_TO_U16(pdata + 2),
					LE_TO_U16(pdata + 4), LE_TO_U16(pdata + 6), LE_TO_U16(pdata + 8));
			BT_AT_PRINT("+BLEMESHCONFIG:compo_data_get,p0,0,0x%04x,0x%04x,0x%04x,0x%04x,%d,0x%04x\r\n",
						src, LE_TO_U16(pdata), LE_TO_U16(pdata + 2),
						LE_TO_U16(pdata + 4), LE_TO_U16(pdata + 6), LE_TO_U16(pdata + 8));
			pdata += 10;
			while (pdata < pend) {
				uint8_t sig_model_num = pdata[2];
				uint8_t vendor_model_num = pdata[3];
				uint8_t model_num;
				BT_LOGA("element %d: loc=%d sig=%d vendor=%d\r\n", element_index, LE_TO_U16(pdata),
						sig_model_num, vendor_model_num);
				BT_AT_PRINT("+BLEMESHCONFIG:compo_data_get,element,%d,%d,%d,%d\r\n",
							element_index, LE_TO_U16(pdata), sig_model_num, vendor_model_num);
				pdata += 4;
				if (sig_model_num) {
					BT_LOGA("\ts:");
					BT_AT_PRINT("+BLEMESHCONFIG:compo_data_get,s");
					for (model_num = 0; model_num < sig_model_num; model_num++, pdata += 2) {
						BT_LOGA(" 0x%04xffff", LE_TO_U16(pdata));
						BT_AT_PRINT(",0x%04xffff", LE_TO_U16(pdata));
					}
					BT_LOGA("\r\n");
					BT_AT_PRINT("\r\n");
				}
				if (vendor_model_num) {
					BT_LOGA("\tv:");
					BT_AT_PRINT("+BLEMESHCONFIG:compo_data_get,v");
					for (model_num = 0; model_num < vendor_model_num; model_num++, pdata += 4) {
						BT_LOGA(" 0x%08x", (unsigned int)LE_TO_U32(pdata));
						BT_AT_PRINT(",0x%08x", (unsigned int)LE_TO_U32(pdata));
					}
					BT_LOGA("\r\n");
					BT_AT_PRINT("\r\n");
				}
				element_index++;
			}
		} else {
			BT_LOGE("cdp0 of 0x%04x invalid!\r\n", LE_TO_U16(p_data + 3));
			BT_AT_PRINT("+BLEMESHCONFIG:compo_data_get,p0,-1\r\n");
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_PROXY_STAT: {
		rtk_bt_mesh_cfg_proxy_stat_t *proxy_stat;
		proxy_stat = (rtk_bt_mesh_cfg_proxy_stat_t *)param;
		BT_LOGA("[APP] provisoner receive proxy state: %d\r\n", proxy_stat->state);
		BT_AT_PRINT("+BLEMESHCONFIG:proxy_state,%d\r\n", proxy_stat->state);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_STAT: {
		rtk_bt_mesh_cfg_model_pub_stat_t *model_pub_stat;
		model_pub_stat = (rtk_bt_mesh_cfg_model_pub_stat_t *)param;
		if (model_pub_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Pub stat: pub addr = 0x%04x, ttl = %d, period steps:%d, period resolution:%d, retrans count:%d, retrans steps:%d\r\n",
					model_pub_stat->pub_addr, model_pub_stat->pub_ttl, model_pub_stat->pub_period.steps, model_pub_stat->pub_period.resol,
					model_pub_stat->pub_retrans_info.count, model_pub_stat->pub_retrans_info.steps);
			BT_AT_PRINT("+BLEMESHCONFIG:pub_state,%d,0x%04x,%d,%d,%d,%d,%d\r\n",
						model_pub_stat->stat, model_pub_stat->pub_addr, model_pub_stat->pub_ttl,
						model_pub_stat->pub_period.steps, model_pub_stat->pub_period.resol,
						model_pub_stat->pub_retrans_info.count, model_pub_stat->pub_retrans_info.steps);
		} else {
			BT_LOGE("[APP] Fail, status = %d!\r\n", model_pub_stat->stat);
			BT_AT_PRINT("+BLEMESHCONFIG:pub_state,%d\r\n", model_pub_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_SUB_LIST:
	case RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_SUB_LIST: {
		BT_LOGA("[APP] Config client model receive message:MODEL SUB LIST SUCCESS\r\n");
		uint16_t *model_sub_list = (uint16_t *)param;
		if (model_sub_list[0] == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			uint16_t src = model_sub_list[1];
			uint16_t element_index = model_sub_list[6];
			uint32_t *model_id_pointer = (uint32_t *) &model_sub_list[4];
			uint32_t model_id = *model_id_pointer;
			uint16_t addr_count = model_sub_list[7];
			BT_LOGA("Model sub list: src 0x%04x element idx %d model id 0x%08x\r\n\t", src,
					element_index, model_id);
			BT_DUMP16A("", &model_sub_list[8], addr_count);
			BT_AT_PRINT("+BLEMESHCONFIG:msg,%d,0x%04x,%d,0x%08x\r\n",
						model_sub_list[0], src, element_index, (unsigned int)model_id);
			BT_AT_DUMP16("+BLEMESHCONFIG:model_sub_list", &model_sub_list[8], addr_count);
		} else {
			BT_LOGE("[APP] Fail, status = %d!\r\n", model_sub_list[0]);
			BT_AT_PRINT("+BLEMESHCONFIG:msg,%d\r\n", model_sub_list[0]);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_KEY_STAT: {
		rtk_bt_mesh_cfg_net_key_stat_t *net_key_stat;
		net_key_stat = (rtk_bt_mesh_cfg_net_key_stat_t *)param;
		if (net_key_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Success!\r\n");
		} else {
			BT_LOGE("[APP] Fail, status = %d!\r\n", net_key_stat->stat);
		}
		BT_AT_PRINT("+BLEMESHCONFIG:net_key_state,%d\r\n", net_key_stat->stat);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_KEY_LIST: {
		BT_LOGA("[APP] Config client model receive message:NET_KEY_LIST SUCCESS\r\n");
		uint16_t *net_key_list = (uint16_t *)param;
		if (net_key_list[0] == 1) {
			uint16_t key_count = net_key_list[1];
			BT_LOGA("NetKey List: num = %d!\r\n\t", key_count);
			BT_DUMP16A("", &net_key_list[4], key_count);
			BT_AT_PRINT("+BLEMESHCONFIG:nkg,0,%d\r\n", key_count);
			BT_AT_DUMP16("+BLEMESHCONFIG:net_key_list", &net_key_list[4], key_count);
		} else {
			BT_LOGE("[APP] Fail, len = %d!\r\n", net_key_list[1]);
			BT_AT_PRINT("+BLEMESHCONFIG:nkg,-1,%d\r\n", net_key_list[1]);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_APP_KEY_LIST: {
		BT_LOGA("[APP] Config client model receive message:APP_KEY_LIST SUCCESS\r\n");
		uint16_t *app_key_list = (uint16_t *)param;
		if (app_key_list[0] == 1) {
			uint16_t net_key_index = app_key_list[1];
			uint16_t key_count = app_key_list[4];
			BT_LOGA("AppKey List: NetKeyIndex = 0x%03x num = %d!\r\n\t", net_key_index,
					key_count);
			BT_DUMP16A("", &app_key_list[5], key_count);
			BT_AT_PRINT("+BLEMESHCONFIG:akg,0,0x%03x,%d\r\n", net_key_index, key_count);
			BT_AT_DUMP16("+BLEMESHCONFIG:app_key_list", &app_key_list[5], key_count);
		} else {
			BT_LOGE("[APP] Fail, stat = %d len = %d!\r\n", app_key_list[1], app_key_list[4]);
			BT_AT_PRINT("+BLEMESHCONFIG:akg,-1,%d,%d\r\n", app_key_list[1], app_key_list[4]);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_STAT: {
		rtk_bt_mesh_cfg_node_identity_stat_t *node_identity_stat;
		node_identity_stat = (rtk_bt_mesh_cfg_node_identity_stat_t *)param;
		if (node_identity_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Node identity state: %d on NetKeyIndex 0x%03x\r\n", node_identity_stat->identity,
					node_identity_stat->net_key_index);
			BT_AT_PRINT("+BLEMESHCONFIG:node_identity_state,%d,%d,0x%03x\r\n",
						node_identity_stat->stat, node_identity_stat->identity, node_identity_stat->net_key_index);
		} else {
			BT_LOGE("[APP] Fail, stat = %d!\r\n", node_identity_stat->stat);
			BT_AT_PRINT("+BLEMESHCONFIG:node_identity_state,%d\r\n", node_identity_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_APP_LIST:
	case RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_APP_LIST: {
		BT_LOGA("\r\n [APP] Config client model receive message:MODEL_APP_LIST SUCCESS\r\n");
		uint16_t *model_app_list = (uint16_t *)param;
		if (model_app_list[0] == 1) {

			uint16_t element_index = model_app_list[1];
			uint32_t *model_id_pointer = (uint32_t *) & model_app_list[4];
			uint32_t model_id = *model_id_pointer;
			uint16_t key_count = model_app_list[6];
			BT_LOGA("Model AppKey List: Element Index = %d model id = 0x%08x num = %d!\r\n\t",
					element_index, (unsigned int) model_id, key_count);
			BT_DUMP16A("", &model_app_list[7], key_count);
			BT_AT_PRINT("+BLEMESHCONFIG:mag,0,%d,0x%08x,%d\r\n",
						element_index, (unsigned int) model_id, key_count);
			BT_AT_DUMP16("+BLEMESHCONFIG:model_app_list", &model_app_list[7], key_count);
		} else {
			BT_LOGE("[APP] Fail, stat = %d len = %d!\r\n", model_app_list[1], model_app_list[4]);
			BT_AT_PRINT("+BLEMESHCONFIG:mag,-1,%d,%d\r\n", model_app_list[1], model_app_list[4]);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_FRND_STAT: {
		rtk_bt_mesh_cfg_frnd_stat_t *frnd_stat;
		frnd_stat = (rtk_bt_mesh_cfg_frnd_stat_t *)param;
		BT_LOGA("[APP] frnd state: %d\r\n", frnd_stat->state);
		BT_AT_PRINT("+BLEMESHCONFIG:frnd_state,%d\r\n", frnd_stat->state);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_LPN_POLL_TO_STAT: {
		rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t *lpn_poll_to_stat;
		lpn_poll_to_stat = (rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t *)param;
		BT_LOGA("[APP] fn = 0x%04x lpn = 0x%04x \r\n", lpn_poll_to_stat->lpn_addr,
				lpn_poll_to_stat->poll_to[0] + (lpn_poll_to_stat->poll_to[1] << 8) + (lpn_poll_to_stat->poll_to[2] << 16));
		BT_AT_PRINT("+BLEMESHCONFIG:lptg,0x%04x,0x%04x\r\n",
					lpn_poll_to_stat->lpn_addr,
					lpn_poll_to_stat->poll_to[0] + (lpn_poll_to_stat->poll_to[1] << 8) + (lpn_poll_to_stat->poll_to[2] << 16));
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_STAT: {
		rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t *key_refresh_phase_stat;
		key_refresh_phase_stat = (rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t *)param;
		if (key_refresh_phase_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Node 0x%04x: NetKeyIndex = 0x%03x state = %d\r\n", key_refresh_phase_stat->src,
					key_refresh_phase_stat->net_key_index, key_refresh_phase_stat->state);
			BT_AT_PRINT("+BLEMESHCONFIG:refresh_phase_state,%d,0x%04x,0x%03x,%d\r\n",
						key_refresh_phase_stat->stat, key_refresh_phase_stat->src,
						key_refresh_phase_stat->net_key_index, key_refresh_phase_stat->state);
		} else {
			BT_LOGE("[APP] Fail, status = %d!\r\n", key_refresh_phase_stat->stat);
			BT_AT_PRINT("+BLEMESHCONFIG:refresh_phase_state,%d\r\n", key_refresh_phase_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_HB_PUB_STAT: {
		rtk_bt_mesh_cfg_hb_pub_stat_t *hb_pub_stat;
		hb_pub_stat = (rtk_bt_mesh_cfg_hb_pub_stat_t *)param;
		if (hb_pub_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Hb: pub dst = 0x%04x count_log:%d, period_log:%d, ttl = %d NetKeyIndex = 0x%03x\r\n",
					hb_pub_stat->dst, hb_pub_stat->count_log, hb_pub_stat->period_log, hb_pub_stat->ttl, hb_pub_stat->net_key_index);
			BT_AT_PRINT("+BLEMESHCONFIG:hb_pub_state,%d,0x%04x,%d,%d,%d,0x%03x\r\n",
						hb_pub_stat->stat, hb_pub_stat->dst, hb_pub_stat->count_log,
						hb_pub_stat->period_log, hb_pub_stat->ttl, hb_pub_stat->net_key_index);
		} else {
			BT_LOGE("[APP] Fail, status = %d!\r\n", hb_pub_stat->stat);
			BT_AT_PRINT("+BLEMESHCONFIG:hb_pub_state,%d\r\n", hb_pub_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_HB_SUB_STAT: {
		rtk_bt_mesh_cfg_hb_sub_stat_t *hb_sub_stat;
		hb_sub_stat = (rtk_bt_mesh_cfg_hb_sub_stat_t *)param;
		if (hb_sub_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			BT_LOGA("[APP] Hb: sub src = 0x%04x dst = 0x%04x period_log = %d count_log = %d hops range = [%d, %d]\r\n", hb_sub_stat->src,
					hb_sub_stat->dst, hb_sub_stat->period_log,  hb_sub_stat->count_log, hb_sub_stat->min_hops, hb_sub_stat->max_hops);
			BT_AT_PRINT("+BLEMESHCONFIG:hb_sub_state,%d,0x%04x,0x%04x,%d,%d,%d,%d\r\n",
						hb_sub_stat->stat, hb_sub_stat->src, hb_sub_stat->dst, hb_sub_stat->period_log,
						hb_sub_stat->count_log, hb_sub_stat->min_hops, hb_sub_stat->max_hops);
		} else {
			BT_LOGE("[APP] Fail, status = %d!\r\n", hb_sub_stat->stat);
			BT_AT_PRINT("+BLEMESHCONFIG:hb_sub_state,%d\r\n", hb_sub_stat->stat);
		}
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_onoff_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_EVT_STATUS: {
		rtk_bt_mesh_generic_onoff_client_evt_status_t *onoff_status;
		onoff_status = (rtk_bt_mesh_generic_onoff_client_evt_status_t *)param;
		if (onoff_status->optional) {
			BT_LOGA("[APP] Generic on off client model receive message: src = 0x%x, present = %d, target = %d, remain time = step(%d), \
resolution(%d)\r\n", onoff_status->src, onoff_status->present_on_off, onoff_status->target_on_off,
					onoff_status->remaining_time.num_steps, onoff_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHGOO:%d,0x%x,%d,1,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, onoff_status->src, onoff_status->present_on_off,
						onoff_status->target_on_off, onoff_status->remaining_time.num_steps,
						onoff_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] Generic on off client model receive message: src = 0x%x, present = %d\r\n", onoff_status->src,
					onoff_status->present_on_off);
			BT_AT_PRINT("+BLEMESHGOO:%d,0x%x,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, onoff_status->src, onoff_status->present_on_off);
		}
		break;
	}
	default:
		BT_LOGE("[%s] Unknown event code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
/***BT and BLE Mesh related api shall not be called in datatrans model app callback***/
static uint8_t datatrans_sample_data[DATA_TRANS_DATA_MAX_LEN] = {0xf, 0xd, 0xb, 0x9, 0x7, 0x5, 0x3, 0x1};
static rtk_bt_evt_cb_ret_t ble_mesh_datatrans_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_WRITE: {
		rtk_bt_mesh_datatrans_server_write_event_t *p_data_write = (rtk_bt_mesh_datatrans_server_write_event_t *)param;
		/*
		        // Avoid compile warning: data_len(uint8_t) always <= DATA_TRANS_DATA_MAX_LEN(0xff)
		        if (p_data_write->data_len > DATA_TRANS_DATA_MAX_LEN) {
		            p_data_write->data_len = DATA_TRANS_DATA_MAX_LEN;
		            BT_LOGA("[APP] Datatrans write message len is %d,extend max val %d,only write %d bytes\r\n", p_data_write->data_len, DATA_TRANS_DATA_MAX_LEN,
		                   DATA_TRANS_DATA_MAX_LEN);
		        }
		*/
		memcpy(datatrans_sample_data, p_data_write->data, p_data_write->data_len);
		BT_LOGA("[APP] Remote write %d bytes: ", p_data_write->data_len);
		mesh_data_uart_dump(p_data_write->data, p_data_write->data_len);
		BT_AT_PRINT("+BLEMESHDATA:write,%d,%d,", BT_AT_MESH_ROLE_SERVER, p_data_write->data_len);
		BT_AT_DUMP_HEXN(p_data_write->data, p_data_write->data_len);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_WRITE_STATUS: {
		rtk_bt_mesh_datatrans_client_write_event_t *p_write_status = (rtk_bt_mesh_datatrans_client_write_event_t *)param;
		BT_LOGA("[APP] Datatrans model receive message: Src %d ,written %d bytes, status:%d\r\n", p_write_status->src, p_write_status->written_len,
				p_write_status->status);
		BT_AT_PRINT("+BLEMESHDATA:write,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, p_write_status->src, p_write_status->written_len, p_write_status->status);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_READ_RESULT: {
		rtk_bt_mesh_datatrans_client_read_event_t *p_read_date = (rtk_bt_mesh_datatrans_client_read_event_t *)param;
		BT_LOGA("[APP] Datatrans model receive message,read %d bytes date from remote src %d :\r\n", p_read_date->data_len, p_read_date->src);
		mesh_data_uart_dump(p_read_date->data, p_read_date->data_len);
		BT_AT_PRINT("+BLEMESHDATA:read,%d,%d,%d,",
					BT_AT_MESH_ROLE_CLIENT, p_read_date->src, p_read_date->data_len);
		BT_AT_DUMP_HEXN(p_read_date->data, p_read_date->data_len);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_READ: {
		rtk_bt_mesh_datatrans_server_read_event_t *server_read = (rtk_bt_mesh_datatrans_server_read_event_t *)param;
		/*
		        // Avoid compile warning: data_len(uint8_t) always <= DATA_TRANS_DATA_MAX_LEN(0xff)
		        if (server_read->data_len > DATA_TRANS_DATA_MAX_LEN) {
		            server_read->data_len = DATA_TRANS_DATA_MAX_LEN;
		            BT_LOGA("[APP] Datatrans read message len is %d, extend max val %d, only read %d bytes\r\n", server_read->data_len, DATA_TRANS_DATA_MAX_LEN,
		                   DATA_TRANS_DATA_MAX_LEN);
		        }
		*/
		memcpy(server_read->data, datatrans_sample_data, server_read->data_len);
		BT_LOGA("[APP] Remote read %d bytes data\r\n", server_read->data_len);
		BT_AT_PRINT("+BLEMESHDATA:read,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, server_read->data_len);
		break;
	}

	default:
		BT_LOGE("[%s] Unknown event code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_remote_prov_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_CAPA_STATUS: {
		rtk_bt_mesh_rmt_prov_client_scan_capa_status_t *scan_capa_status = (rtk_bt_mesh_rmt_prov_client_scan_capa_status_t *)param;
		BT_LOGA("[APP] Remote prov scan capability status: src 0x%x, max scan items %d, support active scan %d\r\n", scan_capa_status->src, \
				scan_capa_status->max_scan_items, (int)scan_capa_status->support_active_scan);
		BT_AT_PRINT("+BLEMESHRMT:scan_capa,0x%x,%d,%d\r\n", scan_capa_status->src,
					scan_capa_status->max_scan_items, (int)scan_capa_status->support_active_scan);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_STATUS: {
		rtk_bt_mesh_rmt_prov_client_scan_status_t *scan_status = (rtk_bt_mesh_rmt_prov_client_scan_status_t *)param;
		BT_LOGA("[APP] Remote prov scan status: src 0x%x, status %d, scan state %d, scanned items limit %d, timeout %d\r\n", scan_status->src, \
				scan_status->prov_status, scan_status->scan_status, scan_status->scanned_items_limit, scan_status->timeout);
		BT_AT_PRINT("+BLEMESHRMT:scan,0x%x,%d,%d,%d,%d\r\n", scan_status->src, scan_status->prov_status,
					scan_status->scan_status, scan_status->scanned_items_limit, scan_status->timeout);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_REPORT: {
		rtk_bt_mesh_rmt_prov_client_scan_report_t *scan_report = (rtk_bt_mesh_rmt_prov_client_scan_report_t *)param;
		if (scan_report->uri_hash) {
			BT_LOGA("[APP] Remote prov scan report: src 0x%x, rssi %d, oob %d, uri hash %d, uuid ", scan_report->src, scan_report->rssi, scan_report->oob,
					(int)scan_report->uri_hash);
		} else {
			BT_LOGA("[APP] Remote prov scan report: src 0x%x, rssi %d, oob %d, uuid ", scan_report->src, scan_report->rssi, scan_report->oob);
		}
		mesh_data_uart_dump(scan_report->uuid, 16);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_EXTENDED_SCAN_REPORT: {
		rtk_bt_mesh_rmt_prov_client_extended_scan_report_t *escan_report = (rtk_bt_mesh_rmt_prov_client_extended_scan_report_t *)param;
		BT_LOGA("[APP] Remote prov extended scan report: src %d, oob %d, uuid ", escan_report->src, escan_report->oob);
		mesh_data_uart_dump(escan_report->uuid, 16);
		if (escan_report->adv_structs_len > 0) {
			BT_LOGA("\r\n adv structs ");
			mesh_data_uart_dump((uint8_t *)param + sizeof(rtk_bt_mesh_rmt_prov_client_extended_scan_report_t), escan_report->adv_structs_len);
		}
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_STATUS: {
		rtk_bt_mesh_rmt_prov_client_link_status_t *link_status = (rtk_bt_mesh_rmt_prov_client_link_status_t *)param;
		BT_LOGA("[APP] Remote prov link status: src 0x%x, status %d, link state %d \r\n", link_status->src, link_status->prov_status, link_status->link_status);
		BT_AT_PRINT("+BLEMESHRMT:link_state,0x%x,%d,%d\r\n",
					link_status->src, link_status->prov_status, link_status->link_status);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_REPORT: {
		rtk_bt_mesh_rmt_prov_client_link_report_t *link_report = (rtk_bt_mesh_rmt_prov_client_link_report_t *)param;
		BT_LOGA("[APP] Remote prov link report: src 0x%x, status %d, link state %d", link_report->src, link_report->prov_status, link_report->link_status);
		BT_AT_PRINT("+BLEMESHRMT:link_report,0x%x,%d,%d\r\n",
					link_report->src, link_report->prov_status, link_report->link_status);
		if (RTK_BT_MESH_RMT_PROV_LINK_NOT_CLOSE != link_report->close_reason) {
			BT_LOGA("link close reason:%d\r\n", link_report->close_reason);
		} else {
			BT_LOGA("\r\n");
		}
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_lightness_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_t *lightness_status;
		lightness_status = (rtk_bt_mesh_light_lightness_client_status_t *) param;
		if (lightness_status->optional) {
			BT_LOGA("[APP] light lightness client model receive: src %d, present %d, target %d, remain time step(%d) resolution(%d)\r\n",
					lightness_status->src,
					lightness_status->present_lightness,
					lightness_status->target_lightness,
					lightness_status->remaining_time.num_steps,
					lightness_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLL:ll,%d,%d,%d,1,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, lightness_status->src,
						lightness_status->present_lightness, lightness_status->target_lightness,
						lightness_status->remaining_time.num_steps, lightness_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light lightness client model receive: src %d, present %d\r\n", lightness_status->src,
					lightness_status->present_lightness);
			BT_AT_PRINT("+BLEMESHLL:ll,%d,%d,%d,0\r\n", BT_AT_MESH_ROLE_CLIENT,
						lightness_status->src, lightness_status->present_lightness);
		}
		break;
	}

	case RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_t *lightness_linear_status;
		lightness_linear_status = (rtk_bt_mesh_light_lightness_client_status_t *) param;
		if (lightness_linear_status->optional) {
			BT_LOGA("[APP] light lightness client model receive: src %d, present %d, target %d, remain time step(%d) resolution(%d)\r\n",
					lightness_linear_status->src,
					lightness_linear_status->present_lightness,
					lightness_linear_status->target_lightness,
					lightness_linear_status->remaining_time.num_steps,
					lightness_linear_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLL:lll,%d,%d,%d,1,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, lightness_linear_status->src,
						lightness_linear_status->present_lightness, lightness_linear_status->target_lightness,
						lightness_linear_status->remaining_time.num_steps, lightness_linear_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light lightness client model receive: src %d, present %d\r\n", lightness_linear_status->src,
					lightness_linear_status->present_lightness);
			BT_AT_PRINT("+BLEMESHLL:lll,%d,%d,%d,0\r\n", BT_AT_MESH_ROLE_CLIENT, lightness_linear_status->src,
						lightness_linear_status->present_lightness);
		}
		break;
	}

	case RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_last_t *lightness_last_status;
		lightness_last_status = (rtk_bt_mesh_light_lightness_client_status_last_t *) param;
		BT_LOGA("[APP] light lightness client model receive: src %d, lightness %d\r\n", lightness_last_status->src,
				lightness_last_status->lightness);
		BT_AT_PRINT("+BLEMESHLL:llla,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT,
					lightness_last_status->src, lightness_last_status->lightness);
		break;
	}

	case RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_default_t *lightness_default_status;
		lightness_default_status = (rtk_bt_mesh_light_lightness_client_status_default_t *) param;
		BT_LOGA("[APP] light lightness client model receive: src %d, lightness %d\r\n", lightness_default_status->src,
				lightness_default_status->lightness);
		BT_AT_PRINT("+BLEMESHLL:lld,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT,
					lightness_default_status->src, lightness_default_status->lightness);
		break;
	}

	case RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_range_t *lightness_range_status;
		lightness_range_status = (rtk_bt_mesh_light_lightness_client_status_range_t *) param;
		BT_LOGA("[APP] light lightness client model receive: src %d, status %d, min %d, max %d\r\n",
				lightness_range_status->src, lightness_range_status->status, lightness_range_status->range_min, lightness_range_status->range_max);
		BT_AT_PRINT("+BLEMESHLL:llr,%d,%d,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, lightness_range_status->src,
					lightness_range_status->status, lightness_range_status->range_min, lightness_range_status->range_max);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_ctl_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_ctl_client_status_t *ctl_status;
		ctl_status = (rtk_bt_mesh_light_ctl_client_status_t *)param;
		if (ctl_status->optional) {
			BT_LOGA("[APP] light ctl client model receive: src %d, present lightness %d, present temperature %d, target lightness %d, target temperature %d, remain time step(%d) resolution(%d)\r\n",
					ctl_status->src,
					ctl_status->present_lightness,
					ctl_status->present_temperature,
					ctl_status->target_lightness, ctl_status->target_temperature,
					ctl_status->remaining_time.num_steps,
					ctl_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLCTL:lc,%d,%d,%d,%d,1,%d,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, ctl_status->src, ctl_status->present_lightness,
						ctl_status->present_temperature, ctl_status->target_lightness,
						ctl_status->target_temperature, ctl_status->remaining_time.num_steps,
						ctl_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light ctl client model receive: src %d, present lightness %d, present temperature %d\r\n",
					ctl_status->src, ctl_status->present_lightness, ctl_status->present_temperature);
			BT_AT_PRINT("+BLEMESHLCTL:lc,%d,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, ctl_status->src,
						ctl_status->present_lightness, ctl_status->present_temperature);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_STATUS: {
		rtk_bt_mesh_light_ctl_client_status_temperature_t *ctl_temperature_status;
		ctl_temperature_status = (rtk_bt_mesh_light_ctl_client_status_temperature_t *) param;
		if (ctl_temperature_status->optional) {
			BT_LOGA("[APP] light ctl client model receive: src %d, present temperature %d, present delta_uv %d, target temperature %d, target delta_uv %d, remain time step(%d), resolution(%d)\r\n",
					ctl_temperature_status->src,
					ctl_temperature_status->present_temperature,
					ctl_temperature_status->present_delta_uv,
					ctl_temperature_status->target_temperature, ctl_temperature_status->target_delta_uv,
					ctl_temperature_status->remaining_time.num_steps,
					ctl_temperature_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLCTL:lct,%d,%d,%d,%d,1,%d,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, ctl_temperature_status->src, ctl_temperature_status->present_temperature,
						ctl_temperature_status->present_delta_uv, ctl_temperature_status->target_temperature,
						ctl_temperature_status->target_delta_uv, ctl_temperature_status->remaining_time.num_steps,
						ctl_temperature_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light ctl client model receive: src %d, present temperature %d, present delta_uv %d\r\n",
					ctl_temperature_status->src, ctl_temperature_status->present_temperature, ctl_temperature_status->present_delta_uv);
			BT_AT_PRINT("+BLEMESHLCTL:lct,%d,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, ctl_temperature_status->src,
						ctl_temperature_status->present_temperature, ctl_temperature_status->present_delta_uv);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_STATUS: {
		rtk_bt_mesh_light_ctl_client_status_temperature_range_t *ctl_temperature_range_status;
		ctl_temperature_range_status = (rtk_bt_mesh_light_ctl_client_status_temperature_range_t *) param;
		BT_LOGA("[APP] light ctl client model receive: src %d, status %d, min %d, max %d\r\n",
				ctl_temperature_range_status->src, ctl_temperature_range_status->status,
				ctl_temperature_range_status->range_min, ctl_temperature_range_status->range_max);
		BT_AT_PRINT("+BLEMESHLCTL:lctr,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, ctl_temperature_range_status->src, ctl_temperature_range_status->status,
					ctl_temperature_range_status->range_min, ctl_temperature_range_status->range_max);
	}
	break;
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_STATUS : {
		rtk_bt_mesh_light_ctl_client_status_default_t *ctl_default_status;
		ctl_default_status = (rtk_bt_mesh_light_ctl_client_status_default_t *) param;
		BT_LOGA("[APP] light ctl client model receive: src  %d, lightness %d, temperature %d, delta_uv %d\r\n",
				ctl_default_status->src, ctl_default_status->lightness, ctl_default_status->temperature, ctl_default_status->delta_uv);
		BT_AT_PRINT("+BLEMESHLCTL:lcd,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, ctl_default_status->src, ctl_default_status->lightness,
					ctl_default_status->temperature, ctl_default_status->delta_uv);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_hsl_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_t *hsl_status;
		hsl_status = (rtk_bt_mesh_light_hsl_client_status_t *)param;
		if (hsl_status->optional) {
			BT_LOGA("[APP] light hsl client model receive: src %d, present lightness %d, present hue %d, present saturation %d, remain time step(%d) resolution(%d)\r\n",
					hsl_status->src,
					hsl_status->lightness,
					hsl_status->hue, hsl_status->saturation,
					hsl_status->remaining_time.num_steps,
					hsl_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLHSL:lh,%d,%d,%d,%d,%d,1,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, hsl_status->src, hsl_status->lightness, hsl_status->hue, hsl_status->saturation,
						hsl_status->remaining_time.num_steps, hsl_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light hsl client model receive: src %d, present lightness %d, present hue %d, presnet saturation %d\r\n",
					hsl_status->src, hsl_status->lightness, hsl_status->hue, hsl_status->saturation);
			BT_AT_PRINT("+BLEMESHLHSL:lh,%d,%d,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, hsl_status->src, hsl_status->lightness,
						hsl_status->hue, hsl_status->saturation);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_TARGET_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_t *hsl_target_status;
		hsl_target_status = (rtk_bt_mesh_light_hsl_client_status_t *)param;
		if (hsl_target_status->optional) {
			BT_LOGA("[APP] light hsl client model receive: src %d, target lightness %d, target hue %d, target saturation %d, remain time step(%d) resolution(%d)\r\n",
					hsl_target_status->src,
					hsl_target_status->lightness,
					hsl_target_status->hue, hsl_target_status->saturation,
					hsl_target_status->remaining_time.num_steps,
					hsl_target_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLHSL:lht,%d,%d,%d,%d,%d,1,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, hsl_target_status->src, hsl_target_status->lightness, hsl_target_status->hue,
						hsl_target_status->saturation, hsl_target_status->remaining_time.num_steps,
						hsl_target_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light hsl client model receive: src %d, target lightness %d, target hue %d, target saturation %d\r\n",
					hsl_target_status->src, hsl_target_status->lightness, hsl_target_status->hue, hsl_target_status->saturation);
			BT_AT_PRINT("+BLEMESHLHSL:lht,%d,%d,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, hsl_target_status->src, hsl_target_status->lightness,
						hsl_target_status->hue, hsl_target_status->saturation);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_hue_t *hsl_last_status;
		hsl_last_status = (rtk_bt_mesh_light_hsl_client_status_hue_t *)param;
		if (hsl_last_status->optional) {
			BT_LOGA("[APP] light hsl client model receive: src %d, present hue %d, target hue %d, remain time step(%d) resolution(%d)\r\n",
					hsl_last_status->src,
					hsl_last_status->present_hue,
					hsl_last_status->target_hue,
					hsl_last_status->remaining_time.num_steps,
					hsl_last_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLHSL:lhh,%d,%d,%d,1,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, hsl_last_status->src, hsl_last_status->present_hue,
						hsl_last_status->target_hue, hsl_last_status->remaining_time.num_steps,
						hsl_last_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light hsl client model receive: src %d, present hue %d\r\n",
					hsl_last_status->src, hsl_last_status->present_hue);
			BT_AT_PRINT("+BLEMESHLHSL:lhh,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, hsl_last_status->src, hsl_last_status->present_hue);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_saturation_t *hsl_saturation_status;
		hsl_saturation_status = (rtk_bt_mesh_light_hsl_client_status_saturation_t *)param;
		if (hsl_saturation_status->optional) {
			BT_LOGA("[APP] light hsl client model receive: src %d, present saturation %d, target saturation %d, remain time step(%d) resolution(%d)\r\n",
					hsl_saturation_status->src,
					hsl_saturation_status->present_saturation,
					hsl_saturation_status->target_saturation,
					hsl_saturation_status->remaining_time.num_steps,
					hsl_saturation_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLHSL:lhs,%d,%d,%d,1,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, hsl_saturation_status->src, hsl_saturation_status->present_saturation,
						hsl_saturation_status->target_saturation, hsl_saturation_status->remaining_time.num_steps,
						hsl_saturation_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light hsl client model receive: src %d, present saturation %d\r\n",
					hsl_saturation_status->src, hsl_saturation_status->present_saturation);
			BT_AT_PRINT("+BLEMESHLHSL:lhs,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, hsl_saturation_status->src, hsl_saturation_status->present_saturation);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_default_t *hsl_default_status;
		hsl_default_status = (rtk_bt_mesh_light_hsl_client_status_default_t *)param;
		BT_LOGA("[APP] light hsl client receive: src %d, lightness %d, hue %d, saturation %d\r\n",
				hsl_default_status->src, hsl_default_status->lightness, hsl_default_status->hue, hsl_default_status->saturation);
		BT_AT_PRINT("+BLEMESHLHSL:lhd,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, hsl_default_status->src, hsl_default_status->lightness,
					hsl_default_status->hue, hsl_default_status->saturation);
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_range_t *hsl_range_status;
		hsl_range_status = (rtk_bt_mesh_light_hsl_client_status_range_t *)param;
		BT_LOGA("[APP] light hsl client receive: src %d, status %d, hue min %d, hue max %d, saturation min %d, saturation max %d\r\n",
				hsl_range_status->src, hsl_range_status->status, hsl_range_status->hue_range_min, hsl_range_status->hue_range_max,
				hsl_range_status->saturation_range_min, hsl_range_status->saturation_range_max);
		BT_AT_PRINT("+BLEMESHLHSL:lhr,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, hsl_range_status->src, hsl_range_status->status, hsl_range_status->hue_range_min,
					hsl_range_status->hue_range_max, hsl_range_status->saturation_range_min, hsl_range_status->saturation_range_max);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_xyl_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_xyl_client_status_t *xyl_status;
		xyl_status = (rtk_bt_mesh_light_xyl_client_status_t *)param;
		if (xyl_status->optional) {
			BT_LOGA("[APP] light xyl client receive: src %d, present lightness %d, present xyl_x %d, present xyl_y %d, remain time step(%d) resolution(%d)\r\n",
					xyl_status->src,
					xyl_status->xyl.xyl_lightness,
					xyl_status->xyl.xyl_x, xyl_status->xyl.xyl_y,
					xyl_status->remaining_time.num_steps,
					xyl_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLXYL:lx,%d,%d,%d,%d,%d,1,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, xyl_status->src, xyl_status->xyl.xyl_lightness, xyl_status->xyl.xyl_x,
						xyl_status->xyl.xyl_y, xyl_status->remaining_time.num_steps, xyl_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light xyl client receive: src %d, present lightness %d, present xyl_x %d, presnet xyl_y %d\r\n",
					xyl_status->src, xyl_status->xyl.xyl_lightness, xyl_status->xyl.xyl_x, xyl_status->xyl.xyl_y);
			BT_AT_PRINT("+BLEMESHLXYL:lx,%d,%d,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, xyl_status->src, xyl_status->xyl.xyl_lightness, xyl_status->xyl.xyl_x,
						xyl_status->xyl.xyl_y);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_XYL_TARGET_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_xyl_client_status_t *xyl_target_status;
		xyl_target_status = (rtk_bt_mesh_light_xyl_client_status_t *)param;
		if (xyl_target_status->optional) {
			BT_LOGA("[APP] light xyl client receive: src %d, target lightness %d, target xyl_x %d, target xyl_y %d, remain time step(%d) resolution(%d)\r\n",
					xyl_target_status->src,
					xyl_target_status->xyl.xyl_lightness,
					xyl_target_status->xyl.xyl_x, xyl_target_status->xyl.xyl_y,
					xyl_target_status->remaining_time.num_steps,
					xyl_target_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLXYL:lxt,%d,%d,%d,%d,%d,1,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, xyl_target_status->src, xyl_target_status->xyl.xyl_lightness,
						xyl_target_status->xyl.xyl_x, xyl_target_status->xyl.xyl_y, xyl_target_status->remaining_time.num_steps,
						xyl_target_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light xyl client receive: src %d, target lightness %d, target xyl_x %d, target xyl_y %d\r\n",
					xyl_target_status->src, xyl_target_status->xyl.xyl_lightness, xyl_target_status->xyl.xyl_x, xyl_target_status->xyl.xyl_y);
			BT_AT_PRINT("+BLEMESHLXYL:lxt,%d,%d,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, xyl_target_status->src, xyl_target_status->xyl.xyl_lightness,
						xyl_target_status->xyl.xyl_x, xyl_target_status->xyl.xyl_y);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_xyl_client_status_default_t *xyl_default_status;
		xyl_default_status = (rtk_bt_mesh_light_xyl_client_status_default_t *)param;
		BT_LOGA("[APP] light xyl client receive: src %d, lightness %d, xyl_x %d, xyl_y %d\r\n",
				xyl_default_status->src, xyl_default_status->xyl.xyl_lightness, xyl_default_status->xyl.xyl_x, xyl_default_status->xyl.xyl_y);
		BT_AT_PRINT("+BLEMESHLXYL:lxd,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, xyl_default_status->src, xyl_default_status->xyl.xyl_lightness,
					xyl_default_status->xyl.xyl_x, xyl_default_status->xyl.xyl_y);
	}
	break;
	case RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_xyl_client_status_range_t *xyl_range_status;
		xyl_range_status = (rtk_bt_mesh_light_xyl_client_status_range_t *)param;
		BT_LOGA("[APP] light xyl client receive: src %d, status %d, xyl_x min %d, xyl_x max %d, xyl_y min %d, xyl_y max %d\r\n",
				xyl_range_status->src, xyl_range_status->status, xyl_range_status->xyl_x_range_min, xyl_range_status->xyl_x_range_max,
				xyl_range_status->xyl_y_range_min, xyl_range_status->xyl_y_range_max);
		BT_AT_PRINT("+BLEMESHLXYL:lxr,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, xyl_range_status->src, xyl_range_status->status, xyl_range_status->xyl_x_range_min,
					xyl_range_status->xyl_x_range_max, xyl_range_status->xyl_y_range_min, xyl_range_status->xyl_y_range_max);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_lc_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lc_client_mode_status_t *lc_mode_status;
		lc_mode_status = (rtk_bt_mesh_light_lc_client_mode_status_t *)param;
		BT_LOGA("[APP] light lc client receive: src %d, mode %d\r\n", lc_mode_status->src, lc_mode_status->mode);
		BT_AT_PRINT("+BLEMESHLLC:llcm,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, lc_mode_status->src, lc_mode_status->mode);
	}
	break;
	case RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lc_client_om_status_t *lc_om_status;
		lc_om_status = (rtk_bt_mesh_light_lc_client_om_status_t *)param;
		BT_LOGA("[APP] light lc client receive: src %d, occupancy mode %d\r\n", lc_om_status->src, lc_om_status->mode);
		BT_AT_PRINT("+BLEMESHLLC:llcom,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, lc_om_status->src, lc_om_status->mode);
	}
	break;
	case RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lc_client_light_on_off_status_t *lc_lightonoff_status;
		lc_lightonoff_status = (rtk_bt_mesh_light_lc_client_light_on_off_status_t *)param;
		if (lc_lightonoff_status->optional) {
			BT_LOGA("[APP] light lc client receive: src %d, present on off %d, target on off %d, remain time step(%d) resolution(%d)\r\n",
					lc_lightonoff_status->src,
					lc_lightonoff_status->present_on_off,
					lc_lightonoff_status->target_on_off,
					lc_lightonoff_status->remaining_time.num_steps,
					lc_lightonoff_status->remaining_time.step_resolution);
			BT_AT_PRINT("+BLEMESHLLC:llcloo,%d,%d,%d,1,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, lc_lightonoff_status->src, lc_lightonoff_status->present_on_off,
						lc_lightonoff_status->target_on_off, lc_lightonoff_status->remaining_time.num_steps,
						lc_lightonoff_status->remaining_time.step_resolution);
		} else {
			BT_LOGA("[APP] light lc client receive: src %d, present on off %d\r\n",
					lc_lightonoff_status->src, lc_lightonoff_status->present_on_off);
			BT_AT_PRINT("+BLEMESHLLC:llcloo,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, lc_lightonoff_status->src, lc_lightonoff_status->present_on_off);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lc_client_property_status_t *lc_property_status;
		lc_property_status = (rtk_bt_mesh_light_lc_client_property_status_t *)param;
		BT_LOGA("[APP] light lc client receive: src %d, property id 0x%x, property value %d\r\n",
				lc_property_status->src, lc_property_status->property_id, lc_property_status->property_value);
		BT_AT_PRINT("+BLEMESHLLC:llcp,%d,%d,0x%x,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, lc_property_status->src,
					lc_property_status->property_id, lc_property_status->property_value);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_time_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_TIME_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_time_client_status_t *time_status;
		time_status = (rtk_bt_mesh_time_client_status_t *)param;
		BT_LOGA("[APP] time client receive: src %d, subsecond %d, uncertainty %d, time_authority %d, tai_utc_delta %d, time_zone_offset %d\r\n",
				time_status->src, (time_status->tai_time).subsecond, (time_status->tai_time).uncertainty, (time_status->tai_time).time_authority,
				(time_status->tai_time).tai_utc_delta, (time_status->tai_time).time_zone_offset);
		BT_LOGA("tai_seconds[4]%d tai_seconds[3]%d tai_seconds[2]%d tai_seconds[1]%d tai_seconds[0]%d \r\n",
				(time_status->tai_time).tai_seconds[4],
				(time_status->tai_time).tai_seconds[3],
				(time_status->tai_time).tai_seconds[2],
				(time_status->tai_time).tai_seconds[1],
				(time_status->tai_time).tai_seconds[0]);
		BT_AT_PRINT("+BLEMESHTIME:time,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, time_status->src, (time_status->tai_time).subsecond,
					(time_status->tai_time).uncertainty, (time_status->tai_time).time_authority,
					(time_status->tai_time).tai_utc_delta, (time_status->tai_time).time_zone_offset,
					(time_status->tai_time).tai_seconds[4], (time_status->tai_time).tai_seconds[3],
					(time_status->tai_time).tai_seconds[2], (time_status->tai_time).tai_seconds[1],
					(time_status->tai_time).tai_seconds[0]);
	}
	break;
	case RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_STATUS: {
		rtk_bt_mesh_time_client_status_role_t *role_status;
		role_status = (rtk_bt_mesh_time_client_status_role_t *)param;
		BT_LOGA("[APP] time client receive: role %d\r\n", role_status->role);
		BT_AT_PRINT("+BLEMESHTIME:tr,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, role_status->role);
	}
	break;
	case RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_STATUS: {
		rtk_bt_mesh_time_client_status_zone_t *zone_status;
		zone_status = (rtk_bt_mesh_time_client_status_zone_t *)param;
		BT_LOGA("[APP] time client receive: src %d time_zone_offset_current %d time_zone_offset_new %d\r\n",
				zone_status->src, zone_status->time_zone_offset_current, zone_status->time_zone_offset_new);
		BT_LOGA("tai_of_zone_change[4]%d tai_of_zone_change[3]%d tai_of_zone_change[2]%d tai_of_zone_change[1]%d tai_of_zone_change[0]%d \r\n",
				zone_status->tai_of_zone_change[4],
				zone_status->tai_of_zone_change[3],
				zone_status->tai_of_zone_change[2],
				zone_status->tai_of_zone_change[1],
				zone_status->tai_of_zone_change[0]);
		BT_AT_PRINT("+BLEMESHTIME:tz,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, zone_status->src, zone_status->time_zone_offset_current,
					zone_status->time_zone_offset_new, zone_status->tai_of_zone_change[4],
					zone_status->tai_of_zone_change[3], zone_status->tai_of_zone_change[2],
					zone_status->tai_of_zone_change[1], zone_status->tai_of_zone_change[0]);
	}
	break;
	case RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_STATUS: {
		rtk_bt_mesh_time_client_status_tai_utc_delta_t *delta_status;
		delta_status = (rtk_bt_mesh_time_client_status_tai_utc_delta_t *)param;
		BT_LOGA("[APP] time client receive: src %d, tai utc delta current %d, tai utc delta new %d\r\n",
				delta_status->src, delta_status->tai_utc_delta_current, delta_status->tai_utc_delta_new);
		BT_LOGA("tai_of_delta_change[4]%d tai_of_delta_change[3]%d tai_of_delta_change[2]%d tai_of_delta_change[1]%d tai_of_delta_change[0]%d \r\n",
				delta_status->tai_of_delta_change[4],
				delta_status->tai_of_delta_change[3],
				delta_status->tai_of_delta_change[2],
				delta_status->tai_of_delta_change[1],
				delta_status->tai_of_delta_change[0]);
		BT_AT_PRINT("+BLEMESHTIME:ttud,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, delta_status->src, delta_status->tai_utc_delta_current,
					delta_status->tai_utc_delta_new, delta_status->tai_of_delta_change[4],
					delta_status->tai_of_delta_change[3], delta_status->tai_of_delta_change[2],
					delta_status->tai_of_delta_change[1], delta_status->tai_of_delta_change[0]);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_scheduler_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_scheduler_client_status_t *scheduler_status;
		scheduler_status = (rtk_bt_mesh_scheduler_client_status_t *)param;
		BT_LOGA("[APP] scheduler client receive: src %d, schedulers 0x%4x\r\n",
				scheduler_status->src, scheduler_status->schedulers);
		BT_AT_PRINT("+BLEMESHSCHEDULER:sche,%d,%d,0x%4x\r\n",
					BT_AT_MESH_ROLE_CLIENT, scheduler_status->src, scheduler_status->schedulers);
	}
	break;
	case RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_scheduler_client_status_action_t *action_status;
		action_status = (rtk_bt_mesh_scheduler_client_status_action_t *)param;
		BT_LOGA("[APP] scheduler client receive: " \
				"index = %d, year = %d, month = %d, day = %d, hour = %d, minute = %d, second = %d, " \
				"day_of_week = %d, action = %d, num_steps = %d, step_resolution = %d, scene_number = %d\r\n", \
				(action_status->scheduler).index, (action_status->scheduler).year, (action_status->scheduler).month, \
				(action_status->scheduler).day, (action_status->scheduler).hour, (action_status->scheduler).minute, (action_status->scheduler).second, \
				(action_status->scheduler).day_of_week, (action_status->scheduler).action, (action_status->scheduler).num_steps, \
				(action_status->scheduler).step_resolution, (action_status->scheduler).scene_number);
		BT_AT_PRINT("+BLEMESHSCHEDULER:schea,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, (action_status->scheduler).index, (action_status->scheduler).year,
					(action_status->scheduler).month, (action_status->scheduler).day, (action_status->scheduler).hour,
					(action_status->scheduler).minute, (action_status->scheduler).second, (action_status->scheduler).day_of_week,
					(action_status->scheduler).action, (action_status->scheduler).num_steps, (action_status->scheduler).step_resolution,
					(action_status->scheduler).scene_number);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_scene_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_scene_client_status_t *scene_status;
		scene_status = (rtk_bt_mesh_scene_client_status_t *)param;
		if (scene_status->optional) {
			BT_LOGA("[APP] scene client receive: src %d, status 0x%d, current scene %d, target scene %d, remaining time %d\r\n",
					scene_status->src, scene_status->status, scene_status->current_scene,
					scene_status->target_scene, (scene_status->remaining_time).num_steps);
			BT_AT_PRINT("+BLEMESHSCENE:scene_state,%d,%d,0x%d,%d,1,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, scene_status->src, scene_status->status, scene_status->current_scene,
						scene_status->target_scene, (scene_status->remaining_time).num_steps);
		} else {
			BT_LOGA("[APP] scene client receive: src %d, status 0x%d, current scene %d\r\n",
					scene_status->src, scene_status->status, scene_status->current_scene);
			BT_AT_PRINT("+BLEMESHSCENE:scene_state,%d,%d,0x%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, scene_status->src, scene_status->status, scene_status->current_scene);
		}

	}
	break;
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_STATUS: {
		uint8_t *pmemory = ((uint8_t *)param + 6);
		uint8_t array_len = *((uint8_t *)param + 5);
		uint16_t src = LE_TO_U16((uint8_t *)param);
		rtk_bt_mesh_scene_status_code_t status = *(((uint8_t *)param) + 2);
		uint16_t current_scene = LE_TO_U16(((uint8_t *)param) + 3);
		BT_LOGA("[APP] scene client receive: src %d, status 0x%d, current scene %d, scene array len %d, scene array:\r\n",
				src, status, current_scene, array_len);
		BT_DUMP16A("", pmemory, array_len);
		BT_AT_PRINT("+BLEMESHSCENE:scene_register_state,%d,%d,0x%d,%d,%d",
					BT_AT_MESH_ROLE_CLIENT, src, status, current_scene, array_len);
		BT_AT_DUMP16("", pmemory, array_len);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_default_transition_time_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_transition_time_client_status_t *transition_status;
		transition_status = (rtk_bt_mesh_generic_transition_time_client_status_t *)param;
		BT_LOGA("[APP] generic default transition time client receive: src %d, resolution %d, num steps %d \r\n",
				transition_status->src, transition_status->trans_time.step_resolution, transition_status->trans_time.num_steps);
		BT_AT_PRINT("+BLEMESHGDTT:gdtt,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, transition_status->src,
					transition_status->trans_time.step_resolution, transition_status->trans_time.num_steps);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_level_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_level_client_status_t *level_status;
		level_status = (rtk_bt_mesh_generic_level_client_status_t *)param;
		if (level_status->optional) {
			BT_LOGA("[APP] generic level client receive: src %d, present level %d, target level %d, remaining time %d \r\n",
					level_status->src, level_status->present_level, level_status->target_level, level_status->remaining_time.num_steps);
			BT_AT_PRINT("+BLEMESHGLE:gl,%d,%d,%d,1,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, level_status->src, level_status->present_level,
						level_status->target_level, level_status->remaining_time.num_steps);
		} else {
			BT_LOGA("[APP] generic level client receive: src %d, present level %d \r\n",
					level_status->src, level_status->present_level);
			BT_AT_PRINT("+BLEMESHGLE:gl,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, level_status->src, level_status->present_level);
		}
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_power_on_off_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_power_on_off_client_status_t *gpoo_status;
		gpoo_status = (rtk_bt_mesh_generic_power_on_off_client_status_t *)param;
		BT_LOGA("[APP] generic power on off client receive: src %d, on power up %d \r\n",
				gpoo_status->src, gpoo_status->on_power_up);
		BT_AT_PRINT("+BLEMESHGPOO:gp,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, gpoo_status->src, gpoo_status->on_power_up);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_power_level_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_power_level_client_status_t *gpl_status;
		gpl_status = (rtk_bt_mesh_generic_power_level_client_status_t *)param;
		if (gpl_status->optional) {
			BT_LOGA("[APP] generic power level client receive: src %d, present power %d, target power %d, remaining time %d \r\n",
					gpl_status->src, gpl_status->present_power, gpl_status->target_power, gpl_status->remaining_time.num_steps);
			BT_AT_PRINT("+BLEMESHGPL:gpl,%d,%d,%d,1,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, gpl_status->src, gpl_status->present_power,
						gpl_status->target_power, gpl_status->remaining_time.num_steps);
		} else {
			BT_LOGA("[APP] generic power level client receive: src %d, present power %d \r\n",
					gpl_status->src, gpl_status->present_power);
			BT_AT_PRINT("+BLEMESHGPL:gpl,%d,%d,%d,0\r\n",
						BT_AT_MESH_ROLE_CLIENT, gpl_status->src, gpl_status->present_power);
		}
	}
	break;
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_LAST_STATUS: {
		rtk_bt_mesh_generic_power_level_client_status_simple_t *gpll_status;
		gpll_status = (rtk_bt_mesh_generic_power_level_client_status_simple_t *)param;
		BT_LOGA("[APP] generic power level client receive: src %d, power %d \r\n",
				gpll_status->src, gpll_status->power);
		BT_AT_PRINT("+BLEMESHGPL:gpllg,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, gpll_status->src, gpll_status->power);
	}
	break;
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_STATUS: {
		rtk_bt_mesh_generic_power_level_client_status_simple_t *gpld_status;
		gpld_status = (rtk_bt_mesh_generic_power_level_client_status_simple_t *)param;
		BT_LOGA("[APP] generic power level client receive: src %d, power %d \r\n",
				gpld_status->src, gpld_status->power);
		BT_AT_PRINT("+BLEMESHGPL:gpld,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, gpld_status->src, gpld_status->power);
	}
	break;
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_STATUS: {
		rtk_bt_mesh_generic_power_level_client_status_range_t *gplr_status;
		gplr_status = (rtk_bt_mesh_generic_power_level_client_status_range_t *)param;
		BT_LOGA("[APP] generic power level client receive: src %d, status %d, range min %d, range max %d \r\n",
				gplr_status->src, gplr_status->stat, gplr_status->range_min, gplr_status->range_max);
		BT_AT_PRINT("+BLEMESHGPL:gplr,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, gplr_status->src, gplr_status->stat,
					gplr_status->range_min, gplr_status->range_max);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_battery_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_battery_client_status_t *gb_status;
		gb_status = (rtk_bt_mesh_generic_battery_client_status_t *)param;
		BT_LOGA("[APP] generic battery client receive: src = %d, battery level = %d, time to discharge = %d, \
time to charge = %d, presence = %d, indicator = %d, charging = %d, serviceability = %d\r\n",
				gb_status->src,
				gb_status->battery_level,
				gb_status->time_to_discharge, gb_status->time_to_charge,
				gb_status->flags.presence, gb_status->flags.indicator, gb_status->flags.charging,
				gb_status->flags.serviceability);
		BT_AT_PRINT("+BLEMESHGB:gbg,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, gb_status->src, gb_status->battery_level,
					gb_status->time_to_discharge, gb_status->time_to_charge, gb_status->flags.presence,
					gb_status->flags.indicator, gb_status->flags.charging, gb_status->flags.serviceability);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_location_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_STATUS: {
		rtk_bt_mesh_generic_location_client_status_global_t *global_status;
		global_status = (rtk_bt_mesh_generic_location_client_status_global_t *)param;
		BT_LOGA("[APP] Generic location client receive: src = %d, latitude = %d, longitude = %d, altitude = %d\r\n",
				global_status->src,
				global_status->global.global_latitude,
				global_status->global.global_longitude,
				global_status->global.global_altitude);
		BT_AT_PRINT("+BLEMESHGLO:glg,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, global_status->src, global_status->global.global_latitude,
					global_status->global.global_longitude, global_status->global.global_altitude);
	}
	break;
	case RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_STATUS: {
		rtk_bt_mesh_generic_location_client_status_local_t *local_status;
		local_status = (rtk_bt_mesh_generic_location_client_status_local_t *)param;
		BT_LOGA("[APP] Generic location client receive: src = %d, north = %d, east = %d, altitude = %d, floor = %d, stationary = %d, updata time = %d, precision = %d\r\n",
				local_status->src, local_status->local.local_north, local_status->local.local_east, local_status->local.local_altitude,
				local_status->local.floor_num,
				local_status->local.uncertainty.stationary, local_status->local.uncertainty.update_time,
				local_status->local.uncertainty.precision);
		BT_AT_PRINT("+BLEMESHGLO:gll,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, local_status->src, local_status->local.local_north,
					local_status->local.local_east, local_status->local.local_altitude, local_status->local.floor_num,
					local_status->local.uncertainty.stationary, local_status->local.uncertainty.update_time,
					local_status->local.uncertainty.precision);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_property_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTIES_STATUS: {
		uint16_t src = LE_TO_U16((uint8_t *)param);
		uint8_t num_ids = *((uint8_t *)param + 2);
		uint8_t *pvalue = (uint8_t *)param + 3;
		BT_LOGA("[APP] Generic property client receive: src = %d num_ids = %d\r\n", src, num_ids);
		BT_DUMP16A("user property ids: ", pvalue, num_ids);
		BT_AT_PRINT("+BLEMESHGP:gups,%d,%d,%d", BT_AT_MESH_ROLE_CLIENT, src, num_ids);
		BT_AT_DUMP16("", pvalue, num_ids);
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_STATUS: {
		rtk_bt_mesh_generic_property_client_status_t *user_property_status;
		user_property_status = (rtk_bt_mesh_generic_property_client_status_t *)param;
		uint8_t property_access = *((uint8_t *)param + 4);
		BT_LOGA("[APP] Generic property client receive: src = %d,  property id = 0x%x, property access = %d\r\n",
				user_property_status->src, user_property_status->property_id, property_access);
		uint16_t value_len = LE_TO_U16((uint8_t *)param + 5);
		uint8_t *pvalue = (uint8_t *)param + 7;
		BT_DUMPA("user property values: ", pvalue, value_len);
		BT_AT_PRINT("+BLEMESHGP:gup,%d,%d,0x%x,%d",
					BT_AT_MESH_ROLE_CLIENT, user_property_status->src, user_property_status->property_id, property_access);
		BT_AT_DUMP("", pvalue, value_len);
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTIES_STATUS: {
		uint16_t src = LE_TO_U16((uint8_t *)param);
		uint8_t num_ids = *((uint8_t *)param + 2);
		BT_LOGA("[APP] Generic property client receive: src = %d num_ids = %d\r\n", src, num_ids);
		BT_DUMP16A("admin property ids: ", (uint8_t *)param + 3, num_ids);
		BT_AT_PRINT("+BLEMESHGP:gaps,%d,%d,%d", BT_AT_MESH_ROLE_CLIENT, src, num_ids);
		BT_AT_DUMP16("", (uint8_t *)param + 3, num_ids);
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_STATUS: {
		rtk_bt_mesh_generic_property_client_status_t *admin_property_status;
		admin_property_status = (rtk_bt_mesh_generic_property_client_status_t *)param;
		uint8_t property_access = *((uint8_t *)param + 4);
		BT_LOGA("[APP] Generic property client receive: src = %d,  property id = 0x%x, property access = %d\r\n",
				admin_property_status->src, admin_property_status->property_id, property_access);
		uint16_t value_len = LE_TO_U16((uint8_t *)param + 5);
		uint8_t *pvalue = (uint8_t *)param + 7;
		BT_DUMPA("admin property values: ", pvalue, value_len);
		BT_AT_PRINT("+BLEMESHGP:gap,%d,%d,0x%x,%d",
					BT_AT_MESH_ROLE_CLIENT, admin_property_status->src, admin_property_status->property_id, property_access);
		BT_AT_DUMP("", pvalue, value_len);
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTIES_STATUS: {
		uint16_t src = LE_TO_U16((uint8_t *)param);
		uint8_t num_ids = *((uint8_t *)param + 2);
		uint8_t *pvalue = (uint8_t *)param + 3;
		BT_LOGA("[APP] Generic property client receive: src = %d num_ids = %d\r\n", src, num_ids);
		BT_DUMP16A("manufacturer property ids: ", pvalue, num_ids);
		BT_AT_PRINT("+BLEMESHGP:gmps,%d,%d,%d", BT_AT_MESH_ROLE_CLIENT, src, num_ids);
		BT_AT_DUMP16("", pvalue, num_ids);
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_STATUS: {
		rtk_bt_mesh_generic_property_client_status_t *manu_property_status;
		manu_property_status = (rtk_bt_mesh_generic_property_client_status_t *)param;
		uint8_t property_access = *((uint8_t *)param + 4);
		BT_LOGA("[APP] Generic property client receive: src = %d,  property id = 0x%x, property access = %d\r\n",
				manu_property_status->src, manu_property_status->property_id, property_access);
		uint16_t value_len = LE_TO_U16((uint8_t *)param + 5);
		uint8_t *pvalue = (uint8_t *)param + 7;
		BT_DUMPA("manufacturer property values: ", pvalue, value_len);
		BT_AT_PRINT("+BLEMESHGP:gmp,%d,%d,0x%x,%d",
					BT_AT_MESH_ROLE_CLIENT, manu_property_status->src, manu_property_status->property_id, property_access);
		BT_AT_DUMP("", pvalue, value_len);
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_CLIENT_PROPERTY_STATUS: {
		uint16_t src = LE_TO_U16((uint8_t *)param);
		uint8_t num_ids = *((uint8_t *)param + 2);
		uint8_t *pvalue = (uint8_t *)param + 3;
		BT_LOGA("[APP] Generic property client receive: src = %d num_ids = %d\r\n", src, num_ids);
		BT_DUMP16A("client property ids: ", pvalue, num_ids);
		BT_AT_PRINT("+BLEMESHGP:gcps,%d,%d,%d", BT_AT_MESH_ROLE_CLIENT, src, num_ids);
		BT_AT_DUMP16("", pvalue, num_ids);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_sensor_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SENSOR_DESCRIPTOR_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_descriptor_t *descriptor_status;
		descriptor_status = (rtk_bt_mesh_sensor_client_status_descriptor_t *)param;
		uint8_t *pdata = (uint8_t *)param + 6;
		rtk_bt_mesh_sensor_descriptor_t descriptor;
		BT_AT_PRINT("+BLEMESHSENSOR:sdg,%d,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT,
					descriptor_status->src, descriptor_status->property_id, descriptor_status->num_descriptors);
		if (descriptor_status->property_id != 0 && descriptor_status->num_descriptors == 0) {
			BT_LOGA("[APP] Sensor client receive: src %d, property id %d \r\n", descriptor_status->src, descriptor_status->property_id);
		} else {
			BT_LOGA("[APP] Sensor client receive: src %d, \r\n", descriptor_status->src);
			for (int i = 0; i < descriptor_status->num_descriptors; i++) {
				memcpy(&descriptor, pdata + 8 * i, 8);
				BT_LOGA("property_id: %d, positive_tolerance: %d, negative_tolerance: %d, sampling_function: %d,"\
						"measurement_period: %d, update_interval: %d \r\n", \
						descriptor.property_id, descriptor.positive_tolerance, descriptor.negative_tolerance,
						descriptor.sampling_function, descriptor.measurement_period, descriptor.update_interval);
				BT_AT_PRINT("+BLEMESHSENSOR:sd,%d,%d,%d,%d,%d,%d,%d\r\n",
							BT_AT_MESH_ROLE_CLIENT, descriptor.property_id, descriptor.positive_tolerance, descriptor.negative_tolerance,
							descriptor.sampling_function, descriptor.measurement_period, descriptor.update_interval);
			}
		}
	}
	break;
	case RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_cadence_t *cadence_status;
		cadence_status = (rtk_bt_mesh_sensor_client_status_cadence_t *)param;
		BT_LOGA("[APP] Sensor client receive: src %d, property id %d \r\n", cadence_status->src, cadence_status->property_id);
		if (cadence_status->flag) {
			uint8_t *pdata = (uint8_t *)param + 5;
			uint8_t raw_value_len = *pdata;
			uint8_t fast_cadence_period_divisor = (*(pdata + 1)) & 0x7F;
			uint8_t status_trigger_type = ((*(pdata + 1)) & 0x80) >> 7;
			BT_LOGA("fast_cadence_period_divisor: %d, status_trigger_type :%d \r\n",
					fast_cadence_period_divisor, status_trigger_type);
			BT_AT_PRINT("+BLEMESHSENSOR:sc,%d,%d,%d,%d,%d,%d,",
						BT_AT_MESH_ROLE_CLIENT, cadence_status->src, cadence_status->property_id,
						cadence_status->flag, fast_cadence_period_divisor, status_trigger_type);
			uint8_t trigger_len;
			if (status_trigger_type == 0) {
				trigger_len = raw_value_len;
			} else {
				trigger_len = 2;
			}
			pdata += 2;
			BT_LOGA("status_trigger_delta_down: \r\n");
			mesh_data_uart_dump(pdata, trigger_len);
			BT_AT_DUMP_HEX(pdata, trigger_len);
			pdata += trigger_len;
			BT_LOGA("status_trigger_delta_up: \r\n");
			BT_AT_PRINT(",");
			mesh_data_uart_dump(pdata, trigger_len);
			BT_AT_DUMP_HEX(pdata, trigger_len);
			pdata += trigger_len;
			uint8_t status_min_interval = *pdata;
			BT_LOGA("status_min_interval: %d \r\n", status_min_interval);
			BT_AT_PRINT(",%d,", status_min_interval);
			pdata++;
			BT_LOGA("fast_cadence_low: \r\n");
			mesh_data_uart_dump(pdata, raw_value_len);
			BT_AT_DUMP_HEX(pdata, raw_value_len);
			pdata += raw_value_len;
			BT_LOGA("fast_cadence_high: \r\n");
			BT_AT_PRINT(",");
			mesh_data_uart_dump(pdata, raw_value_len);
			BT_AT_DUMP_HEXN(pdata, raw_value_len);
		} else {
			BT_AT_PRINT("+BLEMESHSENSOR:sc,%d,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, cadence_status->src, cadence_status->property_id, cadence_status->flag);
		}
	}
	break;
	case RTK_BT_MESH_SENSOR_SETTINGS_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_settings_t *settings_status;
		settings_status = (rtk_bt_mesh_sensor_client_status_settings_t *)param;
		BT_LOGA("[APP] Sensor client receive: src %d, property id %d, ids: \r\n", settings_status->src, settings_status->property_id);
		BT_AT_PRINT("+BLEMESHSENSOR:sssg,%d,%d,%d",
					BT_AT_MESH_ROLE_CLIENT, settings_status->src, settings_status->property_id);
		uint8_t *pdata = (uint8_t *)param + 6;
		for (int i = 0; i < settings_status->num_ids; i++) {
			BT_LOGA("%d ", LE_TO_U16(pdata + 2 * i));
			BT_AT_PRINT(",%d", LE_TO_U16(pdata + 2 * i));
		}
		BT_LOGA("\r\n");
		BT_AT_PRINT("\r\n");
	}
	break;
	case RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_setting_t *setting_status;
		setting_status = (rtk_bt_mesh_sensor_client_status_setting_t *)param;
		BT_LOGA("[APP] Sensor client receive: src %d, property id %d, setting property id: %d \r\n",
				setting_status->src, setting_status->property_id, setting_status->setting_property_id);
		if (setting_status->flag) {
			uint8_t *pdata = (uint8_t *)param + 7;
			rtk_bt_mesh_sensor_setting_access_t setting_access = *(pdata + 2);
			pdata += 3;
			uint8_t raw_len = *pdata;
			pdata++;
			BT_LOGA("setting access:%d, setting raw: ", setting_access);
			mesh_data_uart_dump(pdata, raw_len);
			BT_AT_PRINT("+BLEMESHSENSOR:ss,%d,%d,%d,%d,%d,%d,",
						BT_AT_MESH_ROLE_CLIENT, setting_status->src, setting_status->property_id,
						setting_status->setting_property_id, setting_status->flag, setting_access);
			BT_AT_DUMP_HEXN(pdata, raw_len);
		} else {
			BT_AT_PRINT("+BLEMESHSENSOR:ss,%d,%d,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_CLIENT, setting_status->src, setting_status->property_id,
						setting_status->setting_property_id, setting_status->flag);
		}
	}
	break;
	case RTK_BT_MESH_SENSOR_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_t *sensor_status;
		sensor_status = (rtk_bt_mesh_sensor_client_status_t *)param;
		uint8_t *pdata = (uint8_t *)param + 4;
		BT_LOGA("[APP] Sensor client receive: src %d, marshalled_sensor_data_len %d, data: \r\n",
				sensor_status->src, sensor_status->marshalled_sensor_data_len);
		mesh_data_uart_dump(pdata, sensor_status->marshalled_sensor_data_len);
		BT_AT_PRINT("+BLEMESHSENSOR:sg,%d,%d,%d,",
					BT_AT_MESH_ROLE_CLIENT, sensor_status->src, sensor_status->marshalled_sensor_data_len);
		BT_AT_DUMP_HEXN(pdata, sensor_status->marshalled_sensor_data_len);
	}
	break;
	case RTK_BT_MESH_SENSOR_COLUMN_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_column_t *column_status;
		column_status = (rtk_bt_mesh_sensor_client_status_column_t *)param;
		uint8_t *pdata = (uint8_t *)param + 6;
		BT_LOGA("[APP] Sensor client receive: src %d, property_id %d, column_raw_value_len: %d, column value: \r\n",
				column_status->src, column_status->property_id, column_status->column_raw_value_len);
		mesh_data_uart_dump(pdata, column_status->column_raw_value_len);
		BT_AT_PRINT("+BLEMESHSENSOR:scog,%d,%d,%d,%d,",
					BT_AT_MESH_ROLE_CLIENT, column_status->src, column_status->property_id, column_status->column_raw_value_len);
		BT_AT_DUMP_HEXN(pdata, column_status->column_raw_value_len);
	}
	break;
	case RTK_BT_MESH_SENSOR_SERIES_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_series_t *series_status;
		series_status = (rtk_bt_mesh_sensor_client_status_series_t *)param;
		uint8_t *pdata = (uint8_t *)param + 6;
		BT_LOGA("[APP] Sensor client receive: src %d, property_id %d, series_raw_value_len: %d, series value: \r\n",
				series_status->src, series_status->property_id, series_status->series_raw_value_len);
		mesh_data_uart_dump(pdata, series_status->series_raw_value_len);
		BT_AT_PRINT("+BLEMESHSENSOR:sseg,%d,%d,%d,%d,",
					BT_AT_MESH_ROLE_CLIENT, series_status->src, series_status->property_id, series_status->series_raw_value_len);
		BT_AT_DUMP_HEXN(pdata, series_status->series_raw_value_len);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

static rtk_bt_evt_cb_ret_t ble_mesh_health_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_STATUS: {
		rtk_bt_mesh_health_client_status_attention_t *attn_status;
		attn_status = (rtk_bt_mesh_health_client_status_attention_t *)param;
		BT_LOGA("[APP] Health client receive: attention = %d \r\n",
				attn_status->attention);
		BT_AT_PRINT("+BLEMESHHEALTH:ha,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, attn_status->attention);
	}
	break;
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_CURT_STATUS: {
		rtk_bt_mesh_health_client_status_t *health_status;
		health_status = (rtk_bt_mesh_health_client_status_t *)param;
		BT_LOGA("[APP] Health client receive: src = %d, test id = %d, company id = 0x%04x, fault_array:\r\n",
				health_status->src, health_status->test_id, health_status->company_id);
		/*May not work, if not work, need memcpy pointer address*/
		mesh_data_uart_dump(health_status->fault_array, health_status->fault_array_len);
	}
	break;
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_STATUS: {
		rtk_bt_mesh_health_client_status_t *fault_status;
		fault_status = (rtk_bt_mesh_health_client_status_t *)param;
		BT_LOGA("[APP] Health client receive: src = %d, test id = %d, company id = 0x%04x, fault_array:\r\n",
				fault_status->src, fault_status->test_id, fault_status->company_id);
		/*May not work, if not work, need memcpy pointer address*/
		mesh_data_uart_dump(fault_status->fault_array, fault_status->fault_array_len);
		BT_AT_PRINT("+BLEMESHHEALTH:hf,%d,%d,%d,0x%04x,",
					BT_AT_MESH_ROLE_CLIENT, fault_status->src, fault_status->test_id, fault_status->company_id);
		BT_AT_DUMP_HEXN(fault_status->fault_array, fault_status->fault_array_len);
	}
	break;
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_STATUS: {
		rtk_bt_mesh_health_client_status_period_t *period_status;
		period_status = (rtk_bt_mesh_health_client_status_period_t *)param;
		BT_LOGA("[APP] Health client receive: fast period divisor = %d \r\n", period_status->fast_period_divisor);
		BT_AT_PRINT("+BLEMESHHEALTH:hp,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, period_status->fast_period_divisor);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_directed_forwarding_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_STAT: {
		rtk_bt_mesh_directed_control_status_t *ctl_status;
		ctl_status = (rtk_bt_mesh_directed_control_status_t *)param;
		BT_LOGA("[APP] receive directed control status: status %d, net_key_index 0x%04x, ctl %d-%d-%d-%d-%d\r\n",
				ctl_status->status, ctl_status->net_key_index, ctl_status->directed_ctl.directed_forwarding,
				ctl_status->directed_ctl.directed_relay,
				ctl_status->directed_ctl.directed_proxy, ctl_status->directed_ctl.directed_proxy_use_directed_default,
				ctl_status->directed_ctl.directed_friend);
		BT_AT_PRINT("+BLEMESHDF:dcs,%d,%d,0x%04x,%d-%d-%d-%d-%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, ctl_status->status, ctl_status->net_key_index, ctl_status->directed_ctl.directed_forwarding,
					ctl_status->directed_ctl.directed_relay,
					ctl_status->directed_ctl.directed_proxy, ctl_status->directed_ctl.directed_proxy_use_directed_default,
					ctl_status->directed_ctl.directed_friend);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_PATH_METRIC_STAT: {
		rtk_bt_mesh_path_metric_status_t *metric_status;
		metric_status = (rtk_bt_mesh_path_metric_status_t *)param;
		BT_LOGA("[APP] receive path metric status: status %d, net_key_index 0x%04x, metric %d-%d\r\n",
				metric_status->status,
				metric_status->net_key_index, metric_status->path_metric.metric_type, metric_status->path_metric.lifetime);
		BT_AT_PRINT("+BLEMESHDF:pms,%d,%d,0x%04x,%d-%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, metric_status->status,
					metric_status->net_key_index, metric_status->path_metric.metric_type, metric_status->path_metric.lifetime);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_STAT: {
		rtk_bt_mesh_discovery_table_capabilities_status_t *cap_status;
		cap_status = (rtk_bt_mesh_discovery_table_capabilities_status_t *)param;
		BT_LOGA("[APP] receive discovery table capabilities status: status %d, net_key_index 0x%04x, capabilities %d-%d\r\n",
				cap_status->status, cap_status->net_key_index, cap_status->capabilities.max_concurrent_init,
				cap_status->capabilities.max_discovery_table_entries_count);
		BT_AT_PRINT("+BLEMESHDF:dtcs,%d,%d,0x%04x,%d-%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, cap_status->status, cap_status->net_key_index, cap_status->capabilities.max_concurrent_init,
					cap_status->capabilities.max_discovery_table_entries_count);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_STAT: {
		rtk_bt_mesh_forwarding_table_status_t *fd_status;
		fd_status = (rtk_bt_mesh_forwarding_table_status_t *)param;
		BT_LOGA("[APP] receive forwarding table status: status %d, net_key_index 0x%04x, path_origin 0x%04x, dst 0x%04x\r\n",
				fd_status->status, fd_status->net_key_index, fd_status->path_origin, fd_status->dst);
		BT_AT_PRINT("+BLEMESHDF:fts,%d,%d,0x%04x,0x%04x,0x%04x\r\n",
					BT_AT_MESH_ROLE_CLIENT, fd_status->status, fd_status->net_key_index, fd_status->path_origin, fd_status->dst);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_STAT: {
		rtk_bt_mesh_forwarding_table_dependents_status_t *fdd_status;
		fdd_status = (rtk_bt_mesh_forwarding_table_dependents_status_t *)param;
		BT_LOGA("[APP] receive forwarding table dependents status: status %d, net_key_index 0x%04x, path_origin 0x%04x, dst 0x%04x\r\n",
				fdd_status->status, fdd_status->net_key_index, fdd_status->path_origin, fdd_status->dst);
		BT_AT_PRINT("+BLEMESHDF:ftds,%d,%d,0x%04x,0x%04x,0x%04x\r\n",
					BT_AT_MESH_ROLE_CLIENT, fdd_status->status, fdd_status->net_key_index, fdd_status->path_origin, fdd_status->dst);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_STAT: {
		rtk_bt_mesh_forwarding_table_entries_count_status_t *ftec_status;
		ftec_status = (rtk_bt_mesh_forwarding_table_entries_count_status_t *)param;
		BT_LOGA("[APP] receive forwarding table entries count status: status %d, net_key_index 0x%04x, update id %d, fixed entries count %d, none fixed entries count %d\r\n",
				ftec_status->status, ftec_status->net_key_index, ftec_status->count.forwarding_table_update_id,
				ftec_status->count.fixed_path_entries_count, ftec_status->count.non_fixed_path_entries_count);
		BT_AT_PRINT("+BLEMESHDF:ftdcs,%d,%d,0x%04x,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, ftec_status->status, ftec_status->net_key_index, ftec_status->count.forwarding_table_update_id,
					ftec_status->count.fixed_path_entries_count, ftec_status->count.non_fixed_path_entries_count);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_STAT: {
		rtk_bt_mesh_forwarding_table_entries_status_for_call_t *ftesc_status;
		ftesc_status = (rtk_bt_mesh_forwarding_table_entries_status_for_call_t *)param;
		uint8_t *pdata = (uint8_t *)param + sizeof(rtk_bt_mesh_forwarding_table_entries_status_for_call_t);
		uint8_t *pdata_base = pdata;
		uint16_t path_origin = 0x0000;
		uint16_t dst = 0x0000;
		uint16_t forwarding_table_update_counter = 0;
		if (ftesc_status->filter_mask & RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_PATH_ORIGIN_MATCH) {
			path_origin = LE_TO_U16(pdata);
			pdata += 2;
		}

		if (ftesc_status->filter_mask & RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_DST_MATCH) {
			dst = LE_TO_U16(pdata);
			pdata += 2;
		}

		if (RTK_BT_MESH_MSG_STAT_SUCCESS == ftesc_status->status) {
			forwarding_table_update_counter = LE_TO_U16(pdata);
			pdata += 2;
		}

		BT_LOGA("[APP] receive forwarding table entries status: status %d, net_key_index 0x%04x, filter_mask %d, start_index %d, path_origin 0x%04x, dst 0x%04x, counter %d\r\n",
				ftesc_status->status, ftesc_status->net_key_index, ftesc_status->filter_mask, ftesc_status->start_index, path_origin, dst,
				forwarding_table_update_counter);
		BT_AT_PRINT("+BLEMESHDF:ft_entires_stat,%d,%d,0x%04x,%d,%d,0x%04x,0x%04x,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, ftesc_status->status, ftesc_status->net_key_index, ftesc_status->filter_mask, ftesc_status->start_index, path_origin, dst,
					forwarding_table_update_counter);

		while ((pdata - pdata_base + 1) <= ftesc_status->data_len) {
			rtk_bt_mesh_forwarding_table_entry_header_t *pheader = (rtk_bt_mesh_forwarding_table_entry_header_t *)pdata;
			BT_LOGA("entry header: fixed path %d, unicast_dst %d, backward_path_valid %d, bearer_toward_path_origin_indicator %d, bearer_toward_path_target_indicator %d, dependent_origin_list_size_indicator %d, dependent_target_list_size_indicator %d\r\n",
					pheader->fixed_path_flag, pheader->unicast_dst_flag, pheader->backward_path_validated_flag,
					pheader->bearer_toward_path_origin_indicator, pheader->bearer_toward_path_target_indicator,
					pheader->dependent_origin_list_size_indicator, pheader->dependent_target_list_size_indicator);
			BT_AT_PRINT("+BLEMESHDF:ft_entires_stat,entry_header,%d,%d,%d,%d,%d,%d,%d\r\n",
						pheader->fixed_path_flag, pheader->unicast_dst_flag, pheader->backward_path_validated_flag,
						pheader->bearer_toward_path_origin_indicator, pheader->bearer_toward_path_target_indicator,
						pheader->dependent_origin_list_size_indicator, pheader->dependent_target_list_size_indicator);
			pdata += sizeof(rtk_bt_mesh_forwarding_table_entry_header_t);

			if (pheader->fixed_path_flag) {
				uint16_t entry_path_origin;
				uint8_t entry_path_origin_len = 1;
				uint16_t entry_dependent_origin_list_size = 0;
				uint16_t entry_bearer_toward_path_origin = 0;
				uint16_t entry_path_target;
				uint8_t entry_path_target_len = 1;
				uint16_t entry_dependent_target_list_size = 0;
				uint16_t entry_bearer_toward_path_target = 0;

				rtk_bt_mesh_addr_range_t *paddr = (rtk_bt_mesh_addr_range_t *)pdata;
				entry_path_origin = paddr->range_start_access;
				pdata += 2;
				if (paddr->len_present_access) {
					entry_path_origin_len = paddr->range_len;
					pdata += 1;
				}

				if (pheader->dependent_origin_list_size_indicator == 1) {
					entry_dependent_origin_list_size = *(uint8_t *)pdata;
					pdata += 1;
				} else if (pheader->dependent_origin_list_size_indicator == 2) {
					entry_dependent_origin_list_size = LE_TO_U16(pdata);
					pdata += 2;
				}

				if (pheader->bearer_toward_path_origin_indicator == 1) {
					entry_bearer_toward_path_origin = LE_TO_U16(pdata);
					pdata += 2;
				}

				if (pheader->unicast_dst_flag) {
					rtk_bt_mesh_addr_range_t *paddr = (rtk_bt_mesh_addr_range_t *)pdata;
					entry_path_target = paddr->range_start_access;
					pdata += 2;
					if (paddr->len_present_access) {
						entry_path_target_len = paddr->range_len;
						pdata += 1;
					}
				} else {
					entry_path_target = LE_TO_U16(pdata);
					pdata += 2;
				}

				if (pheader->dependent_target_list_size_indicator == 1) {
					entry_dependent_target_list_size = *(uint8_t *)pdata;
					pdata += 1;
				} else if (pheader->dependent_target_list_size_indicator == 2) {
					entry_dependent_target_list_size = LE_TO_U16(pdata);
					pdata += 2;
				}

				if (pheader->bearer_toward_path_target_indicator == 1) {
					entry_bearer_toward_path_target = LE_TO_U16(pdata);
					pdata += 2;
				}

				BT_LOGA("fixed path entry: path_origin 0x%04x, path_origin_elem_num %d, dependent_origin_list_size %d, bearer_toward_path_origin 0x%x, path_target 0x%04x, path_target_elem_num %d, dependent_target_list_size %d, bearer_toward_path_target 0x%x\r\n",
						entry_path_origin, entry_path_origin_len, entry_dependent_origin_list_size,
						entry_bearer_toward_path_origin, entry_path_target, entry_path_target_len,
						entry_dependent_target_list_size, entry_bearer_toward_path_target);
				BT_AT_PRINT("+BLEMESHDF:ft_entires_stat,fixed_path_entry,0x%04x,%d,%d,0x%x,0x%04x,%d,%d,0x%x\r\n",
							entry_path_origin, entry_path_origin_len, entry_dependent_origin_list_size,
							entry_bearer_toward_path_origin, entry_path_target, entry_path_target_len,
							entry_dependent_target_list_size, entry_bearer_toward_path_target);
			} else {
				uint16_t entry_path_origin;
				uint8_t entry_path_origin_len = 1;
				uint16_t entry_dependent_origin_list_size = 0;
				uint16_t entry_bearer_toward_path_origin = 0;
				uint16_t entry_path_target;
				uint8_t entry_path_target_len = 1;
				uint16_t entry_dependent_target_list_size = 0;
				uint16_t entry_bearer_toward_path_target = 0;

				uint8_t lane_counter = *(uint8_t *)pdata;
				pdata += 1;
				uint16_t remain_time = LE_TO_U16(pdata);
				pdata += 2;
				uint8_t forwarding_num = *(uint8_t *)pdata;
				pdata += 1;

				rtk_bt_mesh_addr_range_t *paddr = (rtk_bt_mesh_addr_range_t *)pdata;
				entry_path_origin = paddr->range_start_access;
				pdata += 2;
				if (paddr->len_present_access) {
					entry_path_origin_len = paddr->range_len;
					pdata += 1;
				}

				if (pheader->dependent_origin_list_size_indicator == 1) {
					entry_dependent_origin_list_size = *(uint8_t *)pdata;
					pdata += 1;
				} else if (pheader->dependent_origin_list_size_indicator == 2) {
					entry_dependent_origin_list_size = LE_TO_U16(pdata);
					pdata += 2;
				}

				if (pheader->bearer_toward_path_origin_indicator == 1) {
					entry_bearer_toward_path_origin = LE_TO_U16(pdata);
					pdata += 2;
				}

				if (pheader->unicast_dst_flag) {
					rtk_bt_mesh_addr_range_t *paddr = (rtk_bt_mesh_addr_range_t *)pdata;
					entry_path_target = paddr->range_start_access;
					pdata += 2;
					if (paddr->len_present_access) {
						entry_path_target_len = paddr->range_len;
						pdata += 1;
					}
				} else {
					entry_path_target = LE_TO_U16(pdata);
					pdata += 2;
				}

				if (pheader->dependent_target_list_size_indicator == 1) {
					entry_dependent_target_list_size = *(uint8_t *)pdata;
					pdata += 1;
				} else if (pheader->dependent_target_list_size_indicator == 2) {
					entry_dependent_target_list_size = LE_TO_U16(pdata);
					pdata += 2;
				}

				if (pheader->bearer_toward_path_target_indicator == 1) {
					entry_bearer_toward_path_target = LE_TO_U16(pdata);
					pdata += 2;
				}

				BT_LOGA("non-fixed path entry: lane_counter %d, remain_time %d, forwarding_num %d, path_origin 0x%04x, path_origin_elem_num %d, dependent_origin_list_size %d, bearer_toward_path_origin 0x%x, path_target 0x%04x, path_target_elem_num %d, dependent_target_list_size %d, bearer_toward_path_target 0x%x\r\n",
						lane_counter, remain_time, forwarding_num, entry_path_origin, entry_path_origin_len,
						entry_dependent_origin_list_size, entry_bearer_toward_path_origin, entry_path_target,
						entry_path_target_len, entry_dependent_target_list_size, entry_bearer_toward_path_target);
				BT_AT_PRINT("+BLEMESHDF:ft_entires_stat,non_fixed_path_entry,%d,%d,%d,0x%04x,%d,%d,0x%x,0x%04x,%d,%d,0x%x\r\n",
							entry_path_origin, entry_path_origin_len, entry_dependent_origin_list_size,
							entry_bearer_toward_path_origin, entry_path_target, entry_path_target_len,
							entry_dependent_target_list_size, entry_bearer_toward_path_target);
			}
		}

	}
	break;
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET_STAT: {
		rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t *ftdg_status;
		ftdg_status = (rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t *)param;
		uint8_t *pdata = (uint8_t *)param + sizeof(rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t);
		uint8_t *pdata_base = pdata;
		uint16_t forwarding_table_update_counter = 0;
		uint16_t dependent_origin_list_size = 0;
		uint16_t dependent_target_list_size = 0;
		if (ftdg_status->status == RTK_BT_MESH_MSG_STAT_SUCCESS || ftdg_status->status == RTK_BT_MESH_MSG_STAT_OBSOLETE_INFORMATION) {
			forwarding_table_update_counter = LE_TO_U16(pdata);
			pdata += 2;
		}

		if (ftdg_status->status == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			dependent_origin_list_size = *pdata;
			pdata += 1;

			dependent_target_list_size = *pdata;
			pdata += 1;
		}
		BT_LOGA("[APP] receive forwarding table dependents get status: status %d, net_key_index 0x%04x, dependents_list_mask 0x%02x, fixed_path_flag %d, start_index %d, path_origin 0x%04x, dst 0x%04x, forwarding_table_update_counter %d, dependents_origin_list_size %d, dependents_target_list_size %d\r\n",
				ftdg_status->status, ftdg_status->net_key_index, ftdg_status->dependents_list_mask, ftdg_status->fixed_path_flag, ftdg_status->status,
				ftdg_status->path_origin, ftdg_status->dst, forwarding_table_update_counter, dependent_origin_list_size,
				dependent_target_list_size);
		BT_AT_PRINT("+BLEMESHDF:ftdgs,%d,%d,0x%04x,0x%02x,%d,%d,0x%04x,0x%04x,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, ftdg_status->status, ftdg_status->net_key_index, ftdg_status->dependents_list_mask, ftdg_status->fixed_path_flag, ftdg_status->status,
					ftdg_status->path_origin, ftdg_status->dst, forwarding_table_update_counter, dependent_origin_list_size,
					dependent_target_list_size);

		uint16_t addr;
		uint8_t addr_len;
		while ((pdata - pdata_base) < ftdg_status->data_len) {
			addr_len = 0;
			rtk_bt_mesh_addr_range_t *paddr = (rtk_bt_mesh_addr_range_t *)pdata;
			addr = paddr->range_start_access;
			pdata += 2;
			if (paddr->len_present_access) {
				addr_len = paddr->range_len;
				pdata += 1;
			} else {
				addr_len = 1;
			}
			BT_LOGA("dependent addr: addr 0x%04x, elem_num %d\r\n", addr, addr_len);
			BT_AT_PRINT(",0x%04x,%d\r\n", addr, addr_len);
		}
	}
	break;
	case RTK_BT_MESH_DF_MODEL_WANTED_LANES_STAT: {
		rtk_bt_mesh_wanted_lanes_status_t *lanes_status;
		lanes_status = (rtk_bt_mesh_wanted_lanes_status_t *)param;
		BT_LOGA("[APP] receive wanted lanes status: status %d, net_key_index 0x%04x, wanted lanes %d\r\n",
				lanes_status->status, lanes_status->net_key_index, lanes_status->wanted_lanes);
		BT_AT_PRINT("+BLEMESHDF:wls,%d,%d,0x%04x,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, lanes_status->status, lanes_status->net_key_index, lanes_status->wanted_lanes);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_STAT: {
		rtk_bt_mesh_two_way_path_status_t *path_status;
		path_status = (rtk_bt_mesh_two_way_path_status_t *)param;
		BT_LOGA("[APP] receive two way path status: status %d, net_key_index 0x%04x, two way path %d\r\n",
				path_status->status, path_status->net_key_index, path_status->two_way_path.two_way_path);
		BT_AT_PRINT("+BLEMESHDF:twps,%d,%d,0x%04x,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, path_status->status, path_status->net_key_index, path_status->two_way_path.two_way_path);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_STAT: {
		rtk_bt_mesh_path_echo_interval_status_t *echo_interval_status;
		echo_interval_status = (rtk_bt_mesh_path_echo_interval_status_t *)param;
		BT_LOGA("[APP] receive path echo interval status: status %d, net_key_index 0x%04x, unicast echo interval %d, multicast echo interval %d\r\n",
				echo_interval_status->status, echo_interval_status->net_key_index, echo_interval_status->unicast_echo_interval, echo_interval_status->multicast_echo_interval);
		BT_AT_PRINT("+BLEMESHDF:peis,%d,%d,0x%04x,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, echo_interval_status->status, echo_interval_status->net_key_index, echo_interval_status->unicast_echo_interval,
					echo_interval_status->multicast_echo_interval);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_STAT: {
		rtk_bt_mesh_directed_network_transmit_status_t *net_trans_status;
		net_trans_status = (rtk_bt_mesh_directed_network_transmit_status_t *)param;
		BT_LOGA("[APP] receive directed network transmit status: count %d, steps %d\r\n",
				net_trans_status->network_transmit.directed_network_transmit_count,
				net_trans_status->network_transmit.directed_network_transmit_interval_steps);
		BT_AT_PRINT("+BLEMESHDF:dnts,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, net_trans_status->network_transmit.directed_network_transmit_count,
					net_trans_status->network_transmit.directed_network_transmit_interval_steps);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_STAT: {
		rtk_bt_mesh_directed_relay_retransmit_status_t *relay_trans_status;
		relay_trans_status = (rtk_bt_mesh_directed_relay_retransmit_status_t *)param;
		BT_LOGA("[APP] receive directed relay retransmit status: count %d, steps %d\r\n",
				relay_trans_status->relay_retransmit.directed_relay_retransmit_count,
				relay_trans_status->relay_retransmit.directed_relay_retransmit_interval_steps);
		BT_AT_PRINT("+BLEMESHDF:drrs,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, relay_trans_status->relay_retransmit.directed_relay_retransmit_count,
					relay_trans_status->relay_retransmit.directed_relay_retransmit_interval_steps);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_STAT: {
		rtk_bt_mesh_rssi_threshold_status_t *rssi_status;
		rssi_status = (rtk_bt_mesh_rssi_threshold_status_t *)param;
		BT_LOGA("[APP] receive rssi threshold status: default_rssi_threshold %d, rssi_margin %d\r\n",
				rssi_status->threshold.default_rssi_threshold, rssi_status->threshold.rssi_margin);
		BT_AT_PRINT("+BLEMESHDF:rts,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, rssi_status->threshold.default_rssi_threshold, rssi_status->threshold.rssi_margin);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_STAT: {
		rtk_bt_mesh_directed_paths_status_t *path_status;
		path_status = (rtk_bt_mesh_directed_paths_status_t *)param;
		BT_LOGA("[APP] receive directed paths status: directed_node_paths %d, directed_relay_paths %d, directed_proxy_paths %d, directed_friend_paths %d\r\n",
				path_status->directed_paths.directed_node_paths, path_status->directed_paths.directed_relay_paths,
				path_status->directed_paths.directed_proxy_paths,
				path_status->directed_paths.directed_friend_paths);
		BT_AT_PRINT("+BLEMESHDF:dps,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, path_status->directed_paths.directed_node_paths,
					path_status->directed_paths.directed_relay_paths,
					path_status->directed_paths.directed_proxy_paths,
					path_status->directed_paths.directed_friend_paths);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_STAT: {
		rtk_bt_mesh_directed_publish_policy_status_t *policy_status;
		policy_status = (rtk_bt_mesh_directed_publish_policy_status_t *)param;
		BT_LOGA("[APP] receive directed publish policy status: status %d, policy %d, elem_addr 0x%04x, model id 0x%08x\r\n",
				policy_status->status, policy_status->publish_policy, policy_status->element_addr, policy_status->model_id);
		BT_AT_PRINT("+BLEMESHDF:dpps,%d,%d,%d,0x%04x,0x%08x\r\n",
					BT_AT_MESH_ROLE_CLIENT, policy_status->status, policy_status->publish_policy, policy_status->element_addr, policy_status->model_id);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_STAT: {
		rtk_bt_mesh_path_discovery_timing_control_status_t *timing_status;
		timing_status = (rtk_bt_mesh_path_discovery_timing_control_status_t *)param;
		BT_LOGA("[APP] receive path discovery timing control status: path_monitoring_interval %d, path_discovery_retry_interval %d, path_discovery_interval %d, lane_discovery_guard_interval %d\r\n",
				timing_status->timing_ctl.path_monitoring_interval, timing_status->timing_ctl.path_discovery_retry_interval,
				timing_status->timing_ctl.path_discovery_interval, timing_status->timing_ctl.lane_discovery_guard_interval);
		BT_AT_PRINT("+BLEMESHDF:pdtcs,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, timing_status->timing_ctl.path_monitoring_interval, timing_status->timing_ctl.path_discovery_retry_interval,
					timing_status->timing_ctl.path_discovery_interval, timing_status->timing_ctl.lane_discovery_guard_interval);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_STAT: {
		rtk_bt_mesh_directed_control_network_transmit_status_t *ctl_net_trans_status;
		ctl_net_trans_status = (rtk_bt_mesh_directed_control_network_transmit_status_t *)param;
		BT_LOGA("[APP] receive directed control network transmit status: count %d, steps %d\r\n",
				ctl_net_trans_status->control_network_transmit.directed_control_network_transmit_count,
				ctl_net_trans_status->control_network_transmit.directed_control_network_transmit_interval_steps);
		BT_AT_PRINT("+BLEMESHDF:dcnts,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, ctl_net_trans_status->control_network_transmit.directed_control_network_transmit_count,
					ctl_net_trans_status->control_network_transmit.directed_control_network_transmit_interval_steps);
	}
	break;
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_STAT: {
		rtk_bt_mesh_directed_control_relay_retransmit_status_t *ctl_relay_trans_status;
		ctl_relay_trans_status = (rtk_bt_mesh_directed_control_relay_retransmit_status_t *)param;
		BT_LOGA("[APP] receive directed control relay retransmit status: count %d, steps %d\r\n",
				ctl_relay_trans_status->control_relay_retransmit.directed_control_relay_retransmit_count,
				ctl_relay_trans_status->control_relay_retransmit.directed_control_relay_retransmit_interval_steps);
		BT_AT_PRINT("+BLEMESHDF:dcrrs,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, ctl_relay_trans_status->control_relay_retransmit.directed_control_relay_retransmit_count,
					ctl_relay_trans_status->control_relay_retransmit.directed_control_relay_retransmit_interval_steps);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_subnet_bridge_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_subnet_bridge_status_t *status;
		status = (rtk_bt_mesh_subnet_bridge_status_t *)param;
		BT_LOGA("[APP] Subnet bridge status: state %d\r\n", status->state);
		BT_AT_PRINT("+BLEMESHSBR:sbrs,%d\r\n", status->state);
	}
	break;
	case RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_bridging_table_status_t *status;
		status = (rtk_bt_mesh_bridging_table_status_t *)param;
		BT_LOGA("[APP] Bridging table status: status %d, directions %d, net_key_index1 0x%04x, net_key_index2 0x%04x, addr1 0x%04x, addr2 0x%04x\r\n",
				status->status, status->directions, status->net_key_index1, status->net_key_index2, status->addr1, status->addr2);
		BT_AT_PRINT("+BLEMESHSBR:sbrbts,%d,%d,%04x,%04x,%04x,%04x\r\n", status->status, status->directions, status->net_key_index1, status->net_key_index2,
					status->addr1, status->addr2);
	}
	break;
	case RTK_BT_MESH_SUBNET_BRIDGE_BRIDGED_SUBNETS_CLIENT_MODEL_LIST: {
		rtk_bt_mesh_bridged_subnets_list_t *list;
		list = (rtk_bt_mesh_bridged_subnets_list_t *)param;
		BT_LOGA("[APP] Bridged subnets list: filter %d, net_key_index 0x%04x, start_index %d\r\n",
				list->filter, list->net_key_index, list->start_index);
		BT_AT_PRINT("+BLEMESHSBR:sbrbrls,%d,%04x,%d\r\n", list->filter, list->net_key_index, list->start_index);
		uint16_t subnets_len = len - sizeof(rtk_bt_mesh_bridged_subnets_list_t);
		uint16_t net_key_index1, net_key_index2;
		uint8_t *psubnets = (uint8_t *)param + sizeof(rtk_bt_mesh_bridged_subnets_list_t);
		for (uint16_t i = 0; i < subnets_len;) {
			net_key_index1 = LE_TO_U16(psubnets + i) & 0x0fff;
			net_key_index2 = (LE_TO_U16(psubnets + i + 1) >> 4);
			BT_LOGA("net key index1 %d, net key index2 %d\r\n", net_key_index1, net_key_index2);
			BT_AT_PRINT("+BLEMESHSBR:bridged_subnets_list,%d,%d", net_key_index1, net_key_index2);
			i += 3;
		}
	}
	break;
	case RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_LIST: {
		rtk_bt_mesh_bridging_table_list_t *list;
		list = (rtk_bt_mesh_bridging_table_list_t *)param;
		BT_LOGA("[APP] Bridging_table_list: status %d, net_key_index1 0x%04x, net_key_index2 0x%04x, start_index %d, bridged_addrs_list \r\n",
				list->status, list->net_key_index1, list->net_key_index2, list->start_index);
		BT_AT_PRINT("+BLEMESHSBR:sbrbtls,%d,%04x,%04x,%d\r\n", list->status, list->net_key_index1, list->net_key_index2, list->start_index);
		uint16_t addrs_len = len - sizeof(rtk_bt_mesh_bridging_table_list_t);
		uint8_t *table_list = (uint8_t *)param + sizeof(rtk_bt_mesh_bridging_table_list_t);
		rtk_bt_mesh_bridged_address_entry_t *pentry = (rtk_bt_mesh_bridged_address_entry_t *)table_list;
		for (uint16_t i = 0; i < addrs_len;) {
			BT_LOGA("addr1 0x%04x, addr2 0x%04x, directions %d\r\n", pentry->addr1, pentry->addr2,
					pentry->directions);
			BT_AT_PRINT("+BLEMESHSBR:bridging_table_list,%04x,%04x,%d", pentry->addr1, pentry->addr2, pentry->directions);
			i += sizeof(rtk_bt_mesh_bridged_address_entry_t);
			pentry += 1;
		}
	}
	break;
	case RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_SIZE: {
		rtk_bt_mesh_bridging_table_size_status_t *status;
		status = (rtk_bt_mesh_bridging_table_size_status_t *)param;
		BT_LOGA("[APP] Bridging table size status: table size %d\r\n", status->bridging_table_size);
		BT_AT_PRINT("+BLEMESHSBR:sbrbts,%d\r\n", status->bridging_table_size);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_private_beacon_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_STATUS: {
		rtk_bt_mesh_private_beacon_status_t *status;
		status = (rtk_bt_mesh_private_beacon_status_t *)param;
		BT_LOGA("Private beacon client receive: private beacon %d, random update interval steps %d\r\n",
				status->private_beacon, status->random_update_interval_steps);
		BT_AT_PRINT("+BLEMESHPRB:prbs,%d,%d\r\n", status->private_beacon, status->random_update_interval_steps);
	}
	break;
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_GATT_PROXY_STATUS: {
		rtk_bt_mesh_private_gatt_proxy_status_t *status;
		status = (rtk_bt_mesh_private_gatt_proxy_status_t *)param;
		BT_LOGA("Private beacon client receive: private gatt proxy %d\r\n",
				status->private_gatt_proxy);
		BT_AT_PRINT("+BLEMESHPRB:pbgps,%d\r\n", status->private_gatt_proxy);
	}
	break;
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_NODE_IDENTITY_STATUS: {
		rtk_bt_mesh_private_node_identity_status_t *status;
		status = (rtk_bt_mesh_private_node_identity_status_t *)param;
		BT_LOGA("Private beacon client receive: status %d, net_key_index 0x%04x, private identity %d\r\n",
				status->status, status->net_key_index, status->private_identity);
		BT_AT_PRINT("+BLEMESHPRB:pbnis,%d\r\n", status->status, status->net_key_index, status->private_identity);
	}
	break;
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
static rtk_bt_evt_cb_ret_t ble_mesh_device_firmware_update_initiator_role_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	BT_LOGD("[%s] Event code:%d.\r\n", __func__, evt_code);
	switch (evt_code) {
	case RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_UPLOAD_STATUS: {
		BT_LOGA("[APP] Receive Firmware Distribution Upload Status message, status:%d.\r\n", *(uint8_t *)param);
		BT_AT_PRINT("+BLEMESHDFU:init_upstarta,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, *(uint8_t *)param);
		break;
	}
	case RTK_BT_MESH_DFU_EVT_INITIATOR_BLOB_PARAM: {
		rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *blob = (rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *)param;
		BT_LOGA("[APP] BLOB param, blob size:%d, block size:%d, total blocks:%d, chunk size:%d.\r\n", blob->blob_size, blob->block_size, blob->total_blocks,
				blob->chunk_size);
		BT_AT_PRINT("+BLEMESHDFU:init_upstartb,%d,%d,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, blob->blob_size, blob->block_size, blob->total_blocks, blob->chunk_size);
		break;
	}
	case RTK_BT_MESH_DFU_EVT_INITIATOR_TRANSFER: {
		rtk_bt_mesh_dfu_evt_initiator_transfer_t *trans = (rtk_bt_mesh_dfu_evt_initiator_transfer_t *)param;
		BT_LOGA("[APP] Transfer upload:");
		if (RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_PROGRESS == trans->type) {
			BT_LOGA("progress, ---------- %d%% ---------", trans->progress);
		} else if (RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_SUCCESS == trans->type) {
			BT_LOGA("success for 0x%x", trans->addr);
		} else if (RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_FAIL == trans->type) {
			BT_LOGA("fail for 0x%x", trans->addr);
		}
		BT_LOGA(", client phase %d.\r\n", trans->init_phase);
		BT_AT_PRINT("+BLEMESHDFU:init_upstartc,%d,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, trans->type,
					trans->type == RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_PROGRESS ? trans->progress : trans->addr, trans->init_phase);
		break;
	}
	case RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_RECEIVES_STATUS: {
		rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status *pstatus = (rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status *)param;
		BT_LOGA("[APP] Receive Firmware Distribution Receivers Status message, status:%d, list count:%d.\r\n", pstatus->status, pstatus->recvs_list_cnt);
		BT_AT_PRINT("+BLEMESHDFU:init_recadd,%d,%d\r\n", pstatus->status, pstatus->recvs_list_cnt);
		break;
	}
	case RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_DISTRIBUTION_STATUS: {
		BT_LOGA("[APP] Receive Firmware Distribution Status message, status:%d.\r\n", *(uint8_t *)param);
		BT_AT_PRINT("+BLEMESHDFU:init_disstarta,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, *(uint8_t *)param);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown event code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif  // BT_MESH_ENABLE_DFU_INITIATOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
static rtk_bt_evt_cb_ret_t ble_mesh_device_firmware_update_standlone_role_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	bool already_print = false;
	BT_LOGD("[%s] Event code:%d.\r\n", __func__, evt_code);
	switch (evt_code) {
	case RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_PARAM: {
		rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *pblob = (rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *)param;
		BT_LOGA("[APP] BLOB param, blob size:%u, block size:%d, total blocks:%d, chunk size:%d\r\n", (unsigned int)pblob->blob_size, pblob->block_size,
				pblob->total_blocks, pblob->chunk_size);
		BT_AT_PRINT("+BLEMESHDFU:stand_starta,%d,%d,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, (unsigned int)pblob->blob_size, pblob->block_size, pblob->total_blocks,
					pblob->chunk_size);
		break;
	}
	case RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_NODE_FAIL: {
		rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t *node_fail = (rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t *)param;
		BT_LOGA("[APP] Type:node fail for addr:0x%x, client phase:%d.\r\n", node_fail->addr, node_fail->dist_phase);
		BT_AT_PRINT("+BLEMESHDFU:stand_startg,%d,%x,%d", BT_AT_MESH_ROLE_CLIENT, node_fail->addr, node_fail->dist_phase);
		break;
	}
	case RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_PROGRESS: {
		rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t *trans_progress = (rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t *)param;
		BT_LOGA("[APP] Type:transfer progress, --------------------- %d%% ----------------, client phase:%d.\r\n", trans_progress->progress,
				trans_progress->dist_phase);
		BT_AT_PRINT("+BLEMESHDFU:stand_startc,%d,%d,%d\r\n", BT_AT_MESH_ROLE_CLIENT, trans_progress->progress, trans_progress->dist_phase);
		break;
	}
	case RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_SUCCESS: {
		if (!already_print) {
			already_print = true;
			BT_LOGA("[APP] Type:transfer success,");
			BT_AT_PRINT("+BLEMESHDFU:stand_startd,%d,", BT_AT_MESH_ROLE_CLIENT);
		}
		__attribute__((fallthrough));
	}
	case RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_VERIFY: {
		if (!already_print) {
			already_print = true;
			BT_LOGA("[APP] Type:verify,");
			BT_AT_PRINT("+BLEMESHDFU:stand_starte,%d,", BT_AT_MESH_ROLE_CLIENT);
		}
		__attribute__((fallthrough));
	}
	case RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_COMPLETE: {
		if (!already_print) {
			already_print = true;
			BT_LOGA("[APP] Type:complete,");
			BT_AT_PRINT("+BLEMESHDFU:stand_starth,%d,", BT_AT_MESH_ROLE_CLIENT);
		}
		rtk_bt_mesh_dfu_evt_distributor_transfer_other_t *trans_other = (rtk_bt_mesh_dfu_evt_distributor_transfer_other_t *)param;
		BT_LOGA(" num of node:%d", trans_other->addr_num);
		BT_AT_PRINT("%d", trans_other->addr_num);
		for (uint8_t i = 0; i < trans_other->addr_num; i++) {
			BT_LOGA(", 0x%x", trans_other->paddr[i]);
			BT_AT_PRINT(",0x%x", trans_other->paddr[i]);
		}
		BT_LOGA(", client phase:%d.\r\n", trans_other->dist_phase);
		BT_AT_PRINT(",%d\r\n", trans_other->dist_phase);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown event code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif  // BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE

static rtk_bt_mesh_stack_act_set_prov_param_t pro_init_param = {
	// Set to 0 to use default val
	.unicast_addr = 0,
	.net_key = {0},
	.app_key = {0},
	/* Or set to the value you want
	.unicast_addr = 0x23,
	.net_key = {0x7d, 0xd7, 0x36, 0x4c, 0xd8, 0x42, 0xad, 0x18, 0xc1, 0x7c, 0x2b, 0x82, 0x0c, 0x84, 0xc3, 0xd6},
	.app_key = {0x63, 0x96, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76, 0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48}
	*/
};

extern uint16_t rtk_bt_mesh_stack_prov_param_set(rtk_bt_mesh_stack_act_set_prov_param_t *init_setting);
int ble_mesh_provisioner_scatternet_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	char addr_str[30] = {0};

	if (1 == enable) {
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_MESH | RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_GATTC;
		bt_app_conf.bt_mesh_app_conf.bt_mesh_role = RTK_BT_MESH_ROLE_PROVISIONER;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.master_init_mtu_req = true;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));

		// MUST DO:set provisioner unicast address,add net key and app key
		BT_APP_PROCESS(rtk_bt_mesh_stack_prov_param_set(&pro_init_param));

		BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
		rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		BT_APP_PROCESS(rtk_bt_le_sm_set_security_param(&sec_param));

		/* gatts related */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS, ble_mesh_gatts_app_callback));
		BT_APP_PROCESS(simple_ble_srv_add());
		BT_APP_PROCESS(device_info_srv_add());
		BT_APP_PROCESS(heart_rate_srv_add());
		BT_APP_PROCESS(battery_srv_add());
		BT_APP_PROCESS(immediate_alert_srv_add());
		BT_APP_PROCESS(glucose_srv_add());
		BT_APP_PROCESS(long_uuid_srv_add());

		/* gattc related */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC, ble_mesh_gattc_app_callback));
		BT_APP_PROCESS(general_client_add());
		BT_APP_PROCESS(bas_client_add());
		BT_APP_PROCESS(gaps_client_add());
		BT_APP_PROCESS(simple_ble_client_add());

		BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(adv_data, sizeof(adv_data)));
		BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));

		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, ble_mesh_gap_app_callback));
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_STACK, ble_mesh_stack_app_callback));
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, ble_mesh_config_client_model_app_callback));
#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL, ble_mesh_generic_onoff_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, ble_mesh_datatrans_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, ble_mesh_remote_prov_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, ble_mesh_light_lightness_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, ble_mesh_light_ctl_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, ble_mesh_light_hsl_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, ble_mesh_light_xyl_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, ble_mesh_light_lc_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, ble_mesh_time_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL, ble_mesh_scheduler_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, ble_mesh_scene_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL,
													ble_mesh_generic_default_transition_time_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL, ble_mesh_generic_level_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL, ble_mesh_generic_power_on_off_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, ble_mesh_generic_power_level_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL, ble_mesh_generic_battery_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL, ble_mesh_generic_location_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, ble_mesh_generic_property_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, ble_mesh_sensor_client_model_app_callback));
#endif
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, ble_mesh_health_client_model_app_callback));
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, ble_mesh_directed_forwarding_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, ble_mesh_subnet_bridge_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, ble_mesh_private_beacon_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, ble_mesh_device_firmware_update_initiator_role_app_callback));
#endif
#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, ble_mesh_device_firmware_update_standlone_role_app_callback));
#endif
	} else if (0 == enable) {
		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());

		app_server_deinit();
		BT_APP_PROCESS(general_client_delete());
		BT_APP_PROCESS(bas_client_delete());
		BT_APP_PROCESS(gaps_client_delete());
		BT_APP_PROCESS(simple_ble_client_delete());
	}

	return 0;
}
