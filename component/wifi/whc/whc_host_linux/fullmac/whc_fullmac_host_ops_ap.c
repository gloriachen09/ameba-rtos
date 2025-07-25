// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

static struct wireless_dev *whc_fullmac_host_add_virtual_intf(struct wiphy *wiphy, const char *name,
		unsigned char name_assign_type, enum nl80211_iftype type, struct vif_params *params)
{
	int ret = 0;
	struct wireless_dev *wdev = NULL;

	dev_info(global_idev.fullmac_dev, "%s: name:%s, type:%d.", __func__, name, type);

	switch (type) {
#ifdef CONFIG_NAN
	case NL80211_IFTYPE_NAN:
		ret = rtw_nan_iface_alloc(wiphy, name, &wdev, params);
		break;
#endif
#ifdef CONFIG_P2P
	case NL80211_IFTYPE_P2P_CLIENT:
	case NL80211_IFTYPE_P2P_GO:
	case NL80211_IFTYPE_P2P_DEVICE:
		ret = whc_fullmac_host_p2p_iface_alloc(wiphy, name, &wdev, type);

		break;
#endif

	default:
		ret = -ENODEV;
		dev_err(global_idev.fullmac_dev, "%s: Unsupported interface type.", __func__);
		break;
	}

	dev_info(global_idev.fullmac_dev, "%s: wdev:%p, ret:%d.", __func__, wdev, ret);

#if defined(CONFIG_NAN) || defined(CONFIG_P2P)
	return wdev ? wdev : ERR_PTR(ret);
#else
	return ERR_PTR(ret);
#endif
}

static int whc_fullmac_host_del_virtual_intf(struct wiphy *wiphy, struct wireless_dev *wdev)
{
	int ret = 0;

#if defined (CONFIG_NAN)
	struct net_device *ndev = wdev_to_ndev(wdev);

	if (wdev->iftype == NL80211_IFTYPE_NAN) {
		dev_info(global_idev.fullmac_dev, "=>"FUNC_NDEV_FMT" - free nan\n", FUNC_NDEV_ARG(ndev));
		if (wdev == global_idev.pwdev_global[2]) {
			rtw_nan_iface_free(wiphy);
		} else {
			dev_err(global_idev.fullmac_dev, "%s: unknown NAN wdev:%p.", __func__, wdev);
		}
	} else
#endif
#ifdef CONFIG_P2P
		if (wdev->iftype == NL80211_IFTYPE_P2P_DEVICE || wdev->iftype == NL80211_IFTYPE_P2P_CLIENT || wdev->iftype == NL80211_IFTYPE_P2P_GO) {
			whc_fullmac_host_p2p_iface_free(wiphy, wdev);
		} else
#endif

		{
			ret = -EINVAL;
			goto exit;
		}

exit:
	return ret;
}

#if defined(CONFIG_RTW_MACADDR_ACL) && (CONFIG_RTW_MACADDR_ACL == 1)
static int whc_fullmac_host_set_mac_acl(struct wiphy *wiphy, struct net_device *ndev, const struct cfg80211_acl_data *params)
{
	return 0;
}
#endif /* CONFIG_RTW_MACADDR_ACL && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)) */

static int whc_fullmac_host_add_station(struct wiphy *wiphy, struct net_device *ndev, const u8 *mac, struct station_parameters *params)
{
	return 0;
}

static int whc_fullmac_host_del_station(struct wiphy *wiphy, struct net_device *ndev, struct station_del_parameters *params)
{
	int ret = 0;
	u8 wlan_idx = rtw_netdev_idx(ndev);
	u8 *mac_vir = NULL;
	dma_addr_t mac_phy;

	if (!params->mac) {
		/*null means delete all sta, not implement right now*/
		return ret;
	}
	dev_dbg(global_idev.fullmac_dev, "[fullmac]:%s %x:%x:%x:%x:%x:%x", __func__,
			params->mac[0], params->mac[1], params->mac[2], params->mac[3], params->mac[4], params->mac[5]);
	mac_vir = rtw_malloc(6, &mac_phy);
	if (!mac_vir) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}
	memcpy(mac_vir, params->mac, 6);
	ret = whc_fullmac_host_ap_del_client(wlan_idx, (u8 *) mac_phy);

	if (mac_vir) {
		rtw_mfree(6, mac_vir, mac_phy);
	}
	return ret;
}

static int whc_fullmac_host_change_station(struct wiphy *wiphy, struct net_device *ndev, const u8 *mac, struct station_parameters *params)
{
	return 0;
}

static int whc_fullmac_host_dump_station(struct wiphy *wiphy, struct net_device *ndev, int idx, u8 *mac, struct station_info *sinfo)
{
	return -ENOENT;
}

static int whc_fullmac_host_change_bss(struct wiphy *wiphy, struct net_device *ndev, struct bss_parameters *params)
{
	return 0;
}

static int whc_fullmac_host_set_txq_params(struct wiphy *wiphy, struct net_device *ndev, struct ieee80211_txq_params *params)
{
	int ret = 0;
	struct rtw_edca_param edca_param = {0};
	u8	shift_count = 0;
	u8 aifsn, aci = 0, ECWMin, ECWMax;
	u16 TXOP;

	switch (params->ac) {
	case NL80211_AC_VO:
		aci = 3;
		break;
	case NL80211_AC_VI:
		aci = 2;
		break;
	case NL80211_AC_BK:
		aci = 1;
		break;
	case NL80211_AC_BE:
		aci = 0;
		break;
	default:
		break;
	}

	while ((params->cwmin + 1) >> shift_count != 1) {
		shift_count++;
		if (shift_count == 15) {
			break;
		}
	}

	ECWMin = shift_count;

	shift_count = 0;
	while ((params->cwmax + 1) >> shift_count != 1) {
		shift_count++;
		if (shift_count == 15) {
			break;
		}
	}

	ECWMax = shift_count;

	TXOP = params->txop;
	aifsn = params->aifs;

	edca_param.aci = aci;
	edca_param.aci_aifsn = aifsn;
	edca_param.cw_max = ECWMax;
	edca_param.cw_min = ECWMin;
	edca_param.txop_limit = TXOP;

	ret = whc_fullmac_host_set_EDCA_params(&edca_param);

	return ret;
}

#ifdef CONFIG_CFG80211_SME_OFFLOAD
static int whc_fullmac_host_probe_client(struct wiphy *wiphy, struct net_device *ndev, const u8 *peer, u64 *cookie)
{
	return 0;
}
#endif // CONFIG_CFG80211_SME_OFFLOAD

#if (LINUX_VERSION_CODE < KERNEL_VERSION(6, 7, 0))
static int whc_fullmac_host_change_beacon(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_beacon_data *info)
#else
static int whc_fullmac_host_change_beacon(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_ap_update *info)
#endif
{
#ifdef CONFIG_P2P
	struct element *target_ptr = NULL;
	struct cfg80211_beacon_data *beacon;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 7, 0))
	beacon = &info->beacon;
#else
	beacon = info;
#endif

	target_ptr = (struct element *)cfg80211_find_vendor_ie(WLAN_OUI_MICROSOFT, WLAN_OUI_TYPE_MICROSOFT_WPS, beacon->beacon_ies, beacon->beacon_ies_len);
	if (target_ptr) {
		whc_fullmac_host_update_custom_ie((u8 *)target_ptr, (global_idev.p2p_global.beacon_wps_ie_idx + 1), (RTW_CUS_IE_BEACON | RTW_CUS_IE_PROBERSP));
	}

	target_ptr = (struct element *)cfg80211_find_vendor_ie(WLAN_OUI_WFA, WLAN_OUI_TYPE_WFA_P2P, beacon->beacon_ies, beacon->beacon_ies_len);
	if (target_ptr) {
		whc_fullmac_host_update_custom_ie((u8 *)target_ptr, (global_idev.p2p_global.beacon_p2p_ie_idx + 1), (RTW_CUS_IE_BEACON | RTW_CUS_IE_PROBERSP));
	}
#endif

	return 0;
}

static int whc_fullmac_host_channel_switch_ops(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_csa_settings *params)
{
	int ret;
	struct rtw_csa_parm *csa_param_vir;
	dma_addr_t csa_param_phy;

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	dev_dbg(global_idev.fullmac_dev, "switch to channel: %d", ieee80211_frequency_to_channel(params->chandef.chan->center_freq));
	dev_dbg(global_idev.fullmac_dev, "switch to freq: %d", params->chandef.chan->center_freq);
	dev_dbg(global_idev.fullmac_dev, "channel switch count: %d", params->count);

	csa_param_vir = rtw_malloc(sizeof(struct rtw_csa_parm), &csa_param_phy);
	if (!csa_param_vir) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	csa_param_vir->new_chl = ieee80211_frequency_to_channel(params->chandef.chan->center_freq);
	csa_param_vir->chl_switch_cnt = params->count;
	/* 0: unicast csa action, 1: broadcast csa action, other values: disable transmit csa action */
	csa_param_vir->action_type = 1;
	/* If set broadcast csa, set broadcast CSA counts. */
	csa_param_vir->bc_action_cnt = 5;
	csa_param_vir->callback = NULL;

	ret = whc_fullmac_host_channel_switch(csa_param_phy);

	if (csa_param_vir) {
		rtw_mfree(sizeof(struct rtw_csa_parm), csa_param_vir, csa_param_phy);
	}

	return ret;
}

void whc_fullmac_host_sta_assoc_indicate(char *buf, int buf_len)
{
	struct station_info sinfo;
	u8 frame_styp, ie_offset = 0;

	memset(&sinfo, 0, sizeof(sinfo));
	frame_styp = le16_to_cpu(((struct rtw_ieee80211_hdr_3addr *)buf)->frame_ctl) & IEEE80211_FCTL_STYPE;
	if (frame_styp == IEEE80211_STYPE_ASSOC_REQ) {
		ie_offset = 4;
	} else { /* RTW_REASSOCREQ */
		ie_offset = 10;
	}
	sinfo.filled = 0;
	sinfo.assoc_req_ies = buf + WLAN_HDR_A3_LEN + 4;
	sinfo.assoc_req_ies_len = buf_len - WLAN_HDR_A3_LEN - 4;
	cfg80211_new_sta(global_idev.pndev[1], get_addr2_ptr(buf), &sinfo, GFP_ATOMIC);
}

static int whc_fullmac_host_start_ap_ops(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_ap_settings *settings)
{
	int ret = 0;
	u8 *buf = NULL, *ptr = NULL;
	size_t size = 0;
	struct rtw_softap_info *softAP_config = NULL;
	char fake_pwd[] = "12345678";
	u8 elem_num = 0;
	const struct element *elem, **pelem;
#ifdef CONFIG_P2P
	struct element *target_ptr = NULL;
	u8 P2P_OUI[4] = {0x50, 0x6f, 0x9a, 0x09};
	u8 WPS_OUI[4] = {0x00, 0x50, 0xf2, 0x04};
#endif
	dev_dbg(global_idev.fullmac_dev, "=>"FUNC_NDEV_FMT" - Start Softap\n", FUNC_NDEV_ARG(ndev));

	if (whc_fullmac_host_dev_driver_is_mp()) {
		return -EPERM;
	}

	if (rtw_netdev_idx(ndev) == 0) {
		return -EPERM;
	}

	size = sizeof(struct rtw_softap_info) + (settings->privacy ? strlen(fake_pwd) + 1 : 0);
	ptr = buf = kmalloc(size, GFP_KERNEL);
	if (!buf) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}
	softAP_config = (struct rtw_softap_info *)ptr;
	ptr += sizeof(struct rtw_softap_info);

	memset(softAP_config, 0, sizeof(struct rtw_softap_info));

	memcpy(softAP_config->ssid.val, (u8 *)settings->ssid, settings->ssid_len);
	softAP_config->ssid.len = settings->ssid_len;
	softAP_config->channel = (u8) ieee80211_frequency_to_channel(settings->chandef.chan->center_freq);
	softAP_config->hidden_ssid = settings->hidden_ssid;

	dev_dbg(global_idev.fullmac_dev, "wpa_versions=%d\n", settings->crypto.wpa_versions);
	dev_dbg(global_idev.fullmac_dev, "n_ciphers_pairwise=%d\n", settings->crypto.n_ciphers_pairwise);
	dev_dbg(global_idev.fullmac_dev, "ciphers_pairwise=0x%x\n", settings->crypto.ciphers_pairwise[0]);
	dev_dbg(global_idev.fullmac_dev, "n_akm_suites=%d\n", settings->crypto.n_akm_suites);
	dev_dbg(global_idev.fullmac_dev, "akm_suites=0x%x\n", settings->crypto.akm_suites[0]);
	dev_dbg(global_idev.fullmac_dev, "cipher_group=0x%x\n", settings->crypto.cipher_group);
#if (KERNEL_VERSION(6, 6, 0) > LINUX_VERSION_CODE)
	dev_dbg(global_idev.fullmac_dev, "wep_tx_key=%d\n", settings->crypto.wep_tx_key);
#endif
	dev_dbg(global_idev.fullmac_dev, "sae_pwd_len=%d\n", settings->crypto.sae_pwd_len);
	dev_dbg(global_idev.fullmac_dev, "hidden_ssid=%d\n", settings->hidden_ssid);

	if (settings->privacy) {
		if (settings->crypto.n_ciphers_pairwise > 1 || settings->crypto.n_akm_suites > 1) {
			dev_dbg(global_idev.fullmac_dev, "wpa mixed mode, not support right now!\n");
			return -EPERM;
		}
		if ((settings->crypto.wpa_versions == 2) && ((u8)settings->crypto.akm_suites[0] == 0x08)) {
			softAP_config->security_type = RTW_SECURITY_WPA3_AES_PSK;
		} else if ((settings->crypto.wpa_versions == 2) && ((u8)settings->crypto.ciphers_pairwise[0] == 0x04)) {
			softAP_config->security_type = RTW_SECURITY_WPA2_AES_PSK;
		} else if ((settings->crypto.wpa_versions == 2) && ((u8)settings->crypto.ciphers_pairwise[0] == 0x02)) {
			softAP_config->security_type = RTW_SECURITY_WPA2_TKIP_PSK;
		} else if (settings->crypto.wpa_versions == 1) {
			dev_dbg(global_idev.fullmac_dev, "wpa_versions=1, not support right now!\n");
			return -EPERM;
		} else {
			softAP_config->security_type = RTW_SECURITY_WEP_PSK;
			dev_err(global_idev.fullmac_dev, "ERR: AP in WEP security mode is not supported!!");
			return -EPERM;
		}

		/* If not fake, copy from upper layer, like WEP(unsupported). */
		/* fix CWE-170, null terminated string, so to add 1. */
		memcpy(ptr, fake_pwd, strlen(fake_pwd) + 1);
		softAP_config->password = (unsigned char *)ptr;
		softAP_config->password_len = strlen(fake_pwd);
		//dev_dbg(global_idev.fullmac_dev, "security_type=0x%x, password=%s, len=%d \n", softAP_config->security_type, softAP_config->password, softAP_config->password_len);
	} else {
		softAP_config->security_type = RTW_SECURITY_OPEN;
	}

#ifdef CONFIG_P2P
	if (settings->beacon.assocresp_ies && settings->beacon.assocresp_ies_len > 0) {
		target_ptr = (struct element *)cfg80211_find_vendor_ie(WLAN_OUI_MICROSOFT, WLAN_OUI_TYPE_MICROSOFT_WPS, settings->beacon.assocresp_ies,
					 settings->beacon.assocresp_ies_len);
		if (target_ptr) {
			whc_fullmac_host_set_gen_ie(1, (u8 *)target_ptr, ((u16)target_ptr->datalen + 2), P2PWPS_ASSOC_RSP_IE);
		}
	}
#endif

	ret = whc_fullmac_host_start_ap(softAP_config);

	netif_carrier_on(ndev);

	if (buf) {
		kfree(buf);
	}

	if (settings->beacon.beacon_ies_len) {
		whc_fullmac_host_del_custom_ie(1);

		/* to get the number of EID_VENDOR_SPECIFIC from beacon_ies */
		elem_num = 0;
		for_each_element_id(elem, WLAN_EID_VENDOR_SPECIFIC,
							settings->beacon.beacon_ies,
							settings->beacon.beacon_ies_len) {
			elem_num++;
		}

		if (elem_num != 0) {
			/* allocate an array to store the pointor of struct element */
			pelem = kmalloc(sizeof(struct element *) * elem_num, GFP_KERNEL);
			if (!pelem) {
				dev_dbg(global_idev.fullmac_dev, "%s: malloc pelem failed.", __func__);
				return -ENOMEM;
			}

			elem_num = 0;
			for_each_element_id(elem, WLAN_EID_VENDOR_SPECIFIC,
								settings->beacon.beacon_ies,
								settings->beacon.beacon_ies_len) {
#ifdef CONFIG_P2P /*record ie_idx for later beacon update*/
				if (elem->datalen >= 4 && !memcmp(elem->data, P2P_OUI, 4)) {
					global_idev.p2p_global.beacon_p2p_ie_idx = elem_num;
				} else if (elem->datalen >= 4 && !memcmp(elem->data, WPS_OUI, 4)) {
					global_idev.p2p_global.beacon_wps_ie_idx = elem_num;
				}
#endif
				pelem[elem_num] = elem;
				elem_num++;
			}

			whc_fullmac_host_add_custom_ie(pelem, elem_num, (RTW_CUS_IE_BEACON | RTW_CUS_IE_PROBERSP));

			kfree(pelem);
		}
	}

	return ret;
}

static int whc_fullmac_host_stop_ap_ops(struct wiphy *wiphy, struct net_device *ndev
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 19, 2))
										, unsigned int link_id
#endif
									   )
{
	int ret = 0;
	dev_dbg(global_idev.fullmac_dev, "=>"FUNC_NDEV_FMT" - Stop Softap\n", FUNC_NDEV_ARG(ndev));

	if (whc_fullmac_host_dev_driver_is_mp()) {
		return -EPERM;
	}

	if (rtw_netdev_idx(ndev) == 0) {
		return -EPERM;
	}

	ret = whc_fullmac_host_stop_ap();

	netif_carrier_off(ndev);
	return ret;
}

int whc_fullmac_host_ap_scan(struct wiphy *wiphy, struct cfg80211_scan_request *request)
{
	int ret = 0;
	struct rtw_scan_param scan_param = {0};
	struct cfg80211_scan_info info;

	dev_dbg(global_idev.fullmac_dev, "whc_fullmac_host_scan enter\n");

	if (whc_fullmac_host_dev_driver_is_mp()) {
		return -EPERM;
	}

	memset(&scan_param, 0, sizeof(struct rtw_scan_param));

	/* Add fake callback to inform rots give scan indicate when scan done. */
	scan_param.scan_user_callback = (s32(*)(u32,  void *))0xffffffff;
	if (request->wdev->iftype == NL80211_IFTYPE_AP) {
		scan_param.scan_report_acs_user_callback = (s32(*)(struct rtw_acs_mntr_rpt *))0xffffffff;
	}
	scan_param.ssid = NULL;

	if (global_idev.mlme_priv.b_in_scan) {
		memset(&info, 0, sizeof(info));
		info.aborted = 0;
		cfg80211_scan_done(request, &info);
		dev_dbg(global_idev.fullmac_dev, "%s: scan is in progress..", __FUNCTION__);
	} else {
		global_idev.mlme_priv.b_in_scan = true;

		ret = whc_fullmac_host_scan(&scan_param, 0, 0);
		if (ret < 0) {
			memset(&info, 0, sizeof(info));
			info.aborted = 0;
			cfg80211_scan_done(request, &info);
			dev_dbg(global_idev.fullmac_dev, "%s: scan request(%p) fail.", __FUNCTION__, request);

			global_idev.mlme_priv.b_in_scan = false;

#ifndef CONFIG_FULLMAC_HCI_IPC
			/* wakeup xmit thread if there are pending packets */
			if (whc_host_xmit_pending_q_num() > 0) {
				whc_host_xmit_wakeup_thread();
			}
#endif
		} else {
			global_idev.mlme_priv.pscan_req_global = request;
			dev_dbg(global_idev.fullmac_dev, "%s: scan request(%p) start.", __FUNCTION__, request);
		}
	}

	return ret;
}

void whc_fullmac_host_ops_ap_init(void)
{
	struct cfg80211_ops *ops = &global_idev.rtw_cfg80211_ops;

	ops->channel_switch = whc_fullmac_host_channel_switch_ops;
	ops->add_virtual_intf = whc_fullmac_host_add_virtual_intf;
	ops->del_virtual_intf = whc_fullmac_host_del_virtual_intf;
	ops->start_ap = whc_fullmac_host_start_ap_ops;
	ops->change_beacon = whc_fullmac_host_change_beacon;
	ops->stop_ap = whc_fullmac_host_stop_ap_ops;
#if defined(CONFIG_RTW_MACADDR_ACL) && (CONFIG_RTW_MACADDR_ACL == 1)
	ops->set_mac_acl = whc_fullmac_host_set_mac_acl;
#endif
	ops->add_station = whc_fullmac_host_add_station;
	ops->del_station = whc_fullmac_host_del_station;
	ops->change_station = whc_fullmac_host_change_station;
	ops->dump_station = whc_fullmac_host_dump_station;
	ops->change_bss = whc_fullmac_host_change_bss;
	ops->set_txq_params = whc_fullmac_host_set_txq_params;
	ops->dump_survey = whc_fullmac_host_dump_survey_params;
#ifdef CONFIG_CFG80211_SME_OFFLOAD
	/*
	 * This is required by AP SAE, otherwise wpa_driver_nl80211_capa() would
	 * set use_monitor to 1 because poll_command_supported is false and
	 * hostap::nl80211_setup_ap would not call nl80211_mgmt_subscribe_ap()
	 * (which SAE AP shall use).
	 */
	ops->probe_client = whc_fullmac_host_probe_client;
#endif	/* CONFIG_CFG80211_SME_OFFLOAD */
}
