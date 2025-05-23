/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BAP_UNICAST_CLIENT_H_
#define _BAP_UNICAST_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "codec_qos.h"
#include "ascs_def.h"
#include "gap_msg.h"
#include "ble_audio_group.h"

/**
 * \defgroup    LEA_GAF_BAP_Unicast_Client BAP Unicast Client
 *
 * \brief   BAP Unicast Client role.
 */

/**
 * \defgroup    BAP_Unicast_Client_Exported_Macros BAP Unicast Client Exported Macros
 *
 * \ingroup LEA_GAF_BAP_Unicast_Client
 * @{
 */

/**
 * bap_unicast_client.h
 *
 * \brief  Define BAP Unicast Server maximum number.
 *
 * \ingroup BAP_Unicast_Client_Exported_Macros
 */
#define BAP_UNICAST_SERVERS_MAX_NUM 2

/**
 * bap_unicast_client.h
 *
 * \brief  Define audio streaming maximum ASE number.
 *
 * \ingroup BAP_Unicast_Client_Exported_Macros
 */
#define AUDIO_STREAM_SRV_ASE_MAX 4

/**
 * End of BAP_Unicast_Client_Exported_Macros
 * @}
 */

/**
 * \defgroup    BAP_Unicast_Client_Exported_Types BAP Unicast Client Exported Types
 *
 * \ingroup LEA_GAF_BAP_Unicast_Client
 * @{
 */

/**
 * bap_unicast_client.h
 *
 * \brief  Define audio streaming session handle.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef void *T_AUDIO_STREAM_SESSION_HANDLE;

/**
 * bap_unicast_client.h
 *
 * \brief  LC3 unicast audio configuration types.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef enum
{
    UNICAST_AUDIO_CFG_UNKNOWN,
    UNICAST_AUDIO_CFG_1,
    UNICAST_AUDIO_CFG_2,
    UNICAST_AUDIO_CFG_3,
    UNICAST_AUDIO_CFG_4,
    UNICAST_AUDIO_CFG_5,
    UNICAST_AUDIO_CFG_6_I,
    UNICAST_AUDIO_CFG_6_II,
    UNICAST_AUDIO_CFG_7_I,
    UNICAST_AUDIO_CFG_7_II,
    UNICAST_AUDIO_CFG_8_I,
    UNICAST_AUDIO_CFG_8_II,
    UNICAST_AUDIO_CFG_9_I,
    UNICAST_AUDIO_CFG_9_II,
    UNICAST_AUDIO_CFG_10,
    UNICAST_AUDIO_CFG_11_I,
    UNICAST_AUDIO_CFG_11_II,
    UNICAST_AUDIO_CFG_MAX
} T_UNICAST_AUDIO_CFG_TYPE;
/**
 * End of BAP_Unicast_Client_Exported_Types
 * @}
 */

/**
 * \defgroup    BAP_Unicast_Client_Exported_Macros BAP Unicast Client Exported Macros
 *
 * \ingroup LEA_GAF_BAP_Unicast_Client
 * @{
 */

/**
 * bap_unicast_client.h
 *
 * \defgroup    LC3_UNICAST_CONFIG LC3 Unicast Audio Configuration Type
 *
 * \brief  Define LC3 unicast audio configuration type bit mask.
 *
 * \ingroup BAP_Unicast_Client_Exported_Macros
 * @{
 */
#define UNICAST_AUDIO_CFG_1_BIT     (1 << UNICAST_AUDIO_CFG_1)
#define UNICAST_AUDIO_CFG_2_BIT     (1 << UNICAST_AUDIO_CFG_2)
#define UNICAST_AUDIO_CFG_3_BIT     (1 << UNICAST_AUDIO_CFG_3)
#define UNICAST_AUDIO_CFG_4_BIT     (1 << UNICAST_AUDIO_CFG_4)
#define UNICAST_AUDIO_CFG_5_BIT     (1 << UNICAST_AUDIO_CFG_5)
#define UNICAST_AUDIO_CFG_6_I_BIT   (1 << UNICAST_AUDIO_CFG_6_I)
#define UNICAST_AUDIO_CFG_6_II_BIT  (1 << UNICAST_AUDIO_CFG_6_II)
#define UNICAST_AUDIO_CFG_7_I_BIT   (1 << UNICAST_AUDIO_CFG_7_I)
#define UNICAST_AUDIO_CFG_7_II_BIT  (1 << UNICAST_AUDIO_CFG_7_II)
#define UNICAST_AUDIO_CFG_8_I_BIT   (1 << UNICAST_AUDIO_CFG_8_I)
#define UNICAST_AUDIO_CFG_8_II_BIT  (1 << UNICAST_AUDIO_CFG_8_II)
#define UNICAST_AUDIO_CFG_9_I_BIT   (1 << UNICAST_AUDIO_CFG_9_I)
#define UNICAST_AUDIO_CFG_9_II_BIT  (1 << UNICAST_AUDIO_CFG_9_II)
#define UNICAST_AUDIO_CFG_10_BIT    (1 << UNICAST_AUDIO_CFG_10)
#define UNICAST_AUDIO_CFG_11_I_BIT  (1 << UNICAST_AUDIO_CFG_11_I)
#define UNICAST_AUDIO_CFG_11_II_BIT (1 << UNICAST_AUDIO_CFG_11_II)
#define UNICAST_AUDIO_CFG_MASK      0x1FFFE
/**
 * End of LC3_UNICAST_CONFIG
 * @}
 */

/**
 * bap_unicast_client.h
 *
 * \brief  Define audio streaming maximum CIS number.
 *
 * \ingroup BAP_Unicast_Client_Exported_Macros
 */
#define AUDIO_STREAM_SRV_CIS_MAX 2
/**
 * End of BAP_Unicast_Client_Exported_Macros
 * @}
 */

/**
 * bap_unicast_client.h
 *
 * \brief  Define Vendor Codec Maximum Length.
 *
 * \ingroup BAP_Unicast_Client_Exported_Macros
 */
#define AUDIO_STREAM_VENDOR_CODEC_MAX_LEN 16
/**
 * End of BAP_Unicast_Client_Exported_Macros
 * \}
 */

/**
 * \defgroup    BAP_Unicast_Client_Exported_Types BAP Unicast Client Exported Types
 *
 * \ingroup LEA_GAF_BAP_Unicast_Client
 * @{
 */

/**
 * bap_unicast_client.h
 *
 * \brief  Audio streaming states.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef enum
{
    AUDIO_STREAM_STATE_IDLE               = 0x00,   /**< Available API: @ref bap_unicast_audio_cfg. */
    AUDIO_STREAM_STATE_IDLE_CONFIGURED    = 0x01,   /**< Available API: @ref bap_unicast_audio_start, @ref bap_unicast_audio_remove_cfg. */
    AUDIO_STREAM_STATE_CONFIGURED         = 0x02,   /**< Available API: @ref bap_unicast_audio_start, @ref bap_unicast_audio_release. */
    AUDIO_STREAM_STATE_STARTING           = 0x03,   /**< Available API: @ref bap_unicast_audio_stop, @ref bap_unicast_audio_release. */
    AUDIO_STREAM_STATE_STREAMING          = 0x04,   /**< Available API: @ref bap_unicast_audio_stop, @ref bap_unicast_audio_release,
                                                         @ref bap_unicast_audio_update. */
    AUDIO_STREAM_STATE_PARTIAL_STREAMING  = 0x05,   /**< Available API: @ref bap_unicast_audio_stop, @ref bap_unicast_audio_release,
                                                         @ref bap_unicast_audio_update. */
    AUDIO_STREAM_STATE_STOPPING           = 0x06,   /**< Available API: @ref bap_unicast_audio_release. */
    AUDIO_STREAM_STATE_RELEASING          = 0x07,
} T_AUDIO_STREAM_STATE;

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast actions.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef enum
{
    BAP_UNICAST_ACTION_IDLE             = 0x00,
    BAP_UNICAST_ACTION_START            = 0x01,
    BAP_UNICAST_ACTION_STOP             = 0x02,
    BAP_UNICAST_ACTION_RELEASE          = 0x03,
    BAP_UNICAST_ACTION_UPDATE           = 0x04,
    BAP_UNICAST_ACTION_REMOVE_CFG       = 0x05,
    BAP_UNICAST_ACTION_REMOVE_SESSION   = 0x06,
    BAP_UNICAST_ACTION_SERVER_STOP      = 0x07,
    BAP_UNICAST_ACTION_SERVER_RELEASE   = 0x08,
} T_BAP_UNICAST_ACTION;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio CIS information.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    uint8_t     cis_id;
    uint16_t    cis_conn_handle;
    uint8_t     data_path_flags;
    uint8_t     sink_ase_id;
    T_ASE_STATE sink_ase_state;
    uint8_t     source_ase_id;
    T_ASE_STATE source_ase_state;
} T_AUDIO_CIS_INFO;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio streaming CIS information.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    uint8_t                cis_num;
    T_AUDIO_CIS_INFO       cis_info[AUDIO_STREAM_SRV_CIS_MAX];
} T_AUDIO_STREAM_CIS_INFO;

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast ASE information.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    uint8_t ase_id;
    uint8_t cfg_idx;
    T_ASE_STATE ase_state;
    uint8_t direction;
    uint8_t channel_num;
} T_BAP_UNICAST_ASE_INFO;

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast device information.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    T_GAP_CONN_STATE       conn_state;
    uint8_t                ase_num;
    T_BAP_UNICAST_ASE_INFO ase_info[AUDIO_STREAM_SRV_ASE_MAX];
} T_BAP_UNICAST_DEV_INFO;

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast session information.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_STATE          state;
    T_UNICAST_AUDIO_CFG_TYPE      cfg_type;
    uint8_t                       conn_dev_num;
    uint8_t                       dev_num;
    T_BAP_UNICAST_DEV_INFO        dev_info[BAP_UNICAST_SERVERS_MAX_NUM];
} T_BAP_UNICAST_SESSION_INFO;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio ASE codec configuration.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    uint8_t              codec_id[CODEC_ID_LEN];
    T_ASE_TARGET_LATENCY target_latency;
    T_ASE_TARGET_PHY     target_phy;
    T_CODEC_CFG          codec_cfg;
    uint8_t              vendor_codec_cfg_len; /**< Only used when Codec_ID does not use LC3_CODEC_ID.
    The parameter is only valid when the bit CODEC_CAP_VENDOR_INFO_EXIST is set in codec_cfg. */
    uint8_t              vendor_codec_cfg[AUDIO_STREAM_VENDOR_CODEC_MAX_LEN]; /**< Only used when
    Codec_ID does not use LC3_CODEC_ID. The parameter is only valid when the bit CODEC_CAP_VENDOR_INFO_EXIST is set in codec_cfg. */
} T_AUDIO_ASE_CODEC_CFG;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio ASE QoS configuration.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    uint8_t  phy;
    uint8_t  retransmission_number;
    uint16_t max_sdu;
} T_AUDIO_ASE_QOS_CFG;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio session QoS configuration.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    uint8_t  sca;
    uint8_t  packing;
    uint8_t  framing;
    uint32_t sdu_interval_m_s;             /**< Length: 3 bytes. */
    uint32_t sdu_interval_s_m;             /**< Length: 3 bytes. */
    uint16_t latency_m_s;
    uint16_t latency_s_m;
    uint32_t sink_presentation_delay;
    uint32_t source_presentation_delay;
} T_AUDIO_SESSION_QOS_CFG;

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast group message result.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef enum
{
    BAP_UNICAST_RESULT_SUCCESS,
    BAP_UNICAST_RESULT_FAILED,
    BAP_UNICAST_RESULT_CIG_SET_ERR,
    BAP_UNICAST_RESULT_CIS_ERR,
    BAP_UNICAST_RESULT_ASE_CP_ERR,
    BAP_UNICAST_RESULT_CIS_DISCONN,
    BAP_UNICAST_RESULT_ASE_INVALID_STATE,
    BAP_UNICAST_RESULT_ACL_DISCONN,
} T_BAP_UNICAST_RESULT;

/**
 * bap_unicast_client.h
 *
 * \brief  BAP_UNICAST_RESULT_ASE_CP_ERR additional information.
 *
 * The additional information data for @ref BAP_UNICAST_RESULT_ASE_CP_ERR.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t opcode;
    uint8_t ase_id;
    uint8_t response_code;
    uint8_t reason;
} T_AUDIO_ASE_CP_ERROR;

/**
 * bap_unicast_client.h
 *
 * \brief  AUDIO_GROUP_MSG_BAP_STATE additional information.
 *
 * The additional information data for @ref AUDIO_GROUP_MSG_BAP_STATE.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef union
{
    T_AUDIO_ASE_CP_ERROR cp_error;
} T_AUDIO_GROUP_ADDL_INFO;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP state information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_STATE.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    T_BAP_UNICAST_ACTION curr_action;
    T_AUDIO_STREAM_STATE state;
    T_BAP_UNICAST_RESULT result;
    uint16_t             cause;
    T_AUDIO_GROUP_ADDL_INFO addl_info;
} T_AUDIO_GROUP_BAP_STATE;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP session remove information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_SESSION_REMOVE.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
} T_AUDIO_GROUP_BAP_SESSION_REMOVE;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP start QoS configuration information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_START_QOS_CFG.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    uint32_t sink_preferred_presentation_delay_min;
    uint32_t sink_preferred_presentation_delay_max;
    uint32_t sink_presentation_delay_min;
    uint32_t sink_presentation_delay_max;
    uint32_t source_preferred_presentation_delay_min;
    uint32_t source_preferred_presentation_delay_max;
    uint32_t source_presentation_delay_min;
    uint32_t source_presentation_delay_max;
    uint16_t sink_transport_latency_max;
    uint16_t source_transport_latency_max;
} T_AUDIO_GROUP_BAP_START_QOS_CFG;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP create CIS information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_CREATE_CIS.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    uint8_t                       cig_id;
    uint16_t                      dev_num;
    uint16_t                      conn_handle_tbl[4];
} T_AUDIO_GROUP_BAP_CREATE_CIS;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP start metadata configuration information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_START_METADATA_CFG.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    uint8_t ase_id;
    uint8_t direction;
} T_AUDIO_GROUP_BAP_START_METADATA_CFG;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP setup data path information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    uint8_t ase_id;
    uint8_t path_direction;                 /**< @ref DATA_PATH_INPUT_FLAG or @ref DATA_PATH_OUTPUT_FLAG. */
    uint16_t cis_conn_handle;
    T_CODEC_CFG codec_parsed_data;
    uint8_t  codec_cfg_len;
    uint8_t *p_codec_cfg;
} T_AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP remove data path information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    uint8_t ase_id;
    uint8_t path_direction;                 /**< @ref DATA_PATH_INPUT_FLAG or @ref DATA_PATH_OUTPUT_FLAG. */
    uint16_t cis_conn_handle;
    uint16_t cause;
} T_AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP CIS disconnect information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_CIS_DISCONN.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    uint16_t                      cause;
    uint16_t                      cis_conn_handle;
    uint16_t                      conn_handle;
    uint8_t                       cig_id;
    uint8_t                       cis_id;
} T_AUDIO_GROUP_BAP_CIS_DISCONN;

/**
 * bap_unicast_client.h
 *
 * \brief  Audio group BAP metadata update information.
 *
 * The message data for @ref AUDIO_GROUP_MSG_BAP_METADATA_UPDATE.
 *
 * \ingroup BAP_Unicast_Client_Exported_Types
 */
typedef struct
{
    T_AUDIO_STREAM_SESSION_HANDLE handle;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    uint8_t ase_id;
    uint8_t direction;
    uint8_t metadata_length;
    uint8_t *p_metadata;
} T_AUDIO_GROUP_MSG_BAP_METADATA_UPDATE;
/**
 * End of BAP_Unicast_Client_Exported_Types
 * @}
 */

/**
 * \defgroup    BAP_Unicast_Client_Exported_Functions BAP Unicast Client Exported Functions
 *
 * \ingroup LEA_GAF_BAP_Unicast_Client
 * @{
 */

/**
 * bap_unicast_client.h
 *
 * \brief  Allocate audio streaming session.
 *
 * \param[in]  group_handle      LE audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 *
 * \return         The audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
T_AUDIO_STREAM_SESSION_HANDLE audio_stream_session_allocate(T_BLE_AUDIO_GROUP_HANDLE group_handle);

/**
 * bap_unicast_client.h
 *
 * \brief  Get LE audio group handle by audio streaming session.
 *
 * \param[in]  handle      Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 *
 * \return         The LE audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
T_BLE_AUDIO_GROUP_HANDLE audio_stream_session_get_group(T_AUDIO_STREAM_SESSION_HANDLE handle);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio get audio configuration mask.
 *
 * \param[in]  handle       Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  prefer_cfg   Preferred configuration.
 * \param[in]  dev_num      The device number of p_dev_tbl.
 * \param[in]  p_dev_tbl    Pointer to device table: @ref T_BLE_AUDIO_DEV_HANDLE.
 *
 * \return         The audio configuration mask.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
uint32_t bap_unicast_get_audio_cfg_mask(T_AUDIO_STREAM_SESSION_HANDLE handle, uint32_t prefer_cfg,
                                        uint8_t dev_num, T_BLE_AUDIO_DEV_HANDLE *p_dev_tbl);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio get cis information.
 *
 * \param[in]  handle      Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  dev_handle  Audio device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \param[in,out]  p_info  Pointer to cis information: @ref T_AUDIO_STREAM_CIS_INFO.
 *
 * \return         The result of BAP unicast audio getting cis information.
 * \retval true    Getting cis information is successful.
 * \retval false   Getting cis information failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_get_cis_info(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                    T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                    T_AUDIO_STREAM_CIS_INFO *p_info);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio get unicast session information.
 *
 * \param[in]  handle       Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in,out]  p_info   Pointer to cis information: @ref T_BAP_UNICAST_SESSION_INFO.
 *
 * \return         The result of BAP unicast audio getting unicast session information.
 * \retval true    Getting unicast session information is successful.
 * \retval false   Getting unicast session information failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_get_session_info(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                        T_BAP_UNICAST_SESSION_INFO *p_info);

/**
 * bap_unicast_client.h
 *
 * \brief  Set BAP unicast audio configuration.
 *
 * \param[in]  handle      Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  cfg_type    Audio configuration type: @ref T_UNICAST_AUDIO_CFG_TYPE.
 * \param[in]  dev_num     The device number of p_dev_tbl.
 * \param[in]  p_dev_tbl   Pointer to device table: @ref T_BLE_AUDIO_DEV_HANDLE.
 *
 * \return         The result of set BAP unicast audio configuration.
 * \retval true    Set BAP unicast audio configuration is successful.
 * \retval false   Set BAP unicast audio configuration failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_cfg(T_AUDIO_STREAM_SESSION_HANDLE handle, T_UNICAST_AUDIO_CFG_TYPE cfg_type,
                           uint8_t dev_num, T_BLE_AUDIO_DEV_HANDLE *p_dev_tbl);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio configure ASE codec.
 *
 * his API is used before @ref bap_unicast_audio_start, and after @ref bap_unicast_audio_cfg.
 *
 * \param[in]  handle      Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  dev_handle  Device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \param[in]  cfg_idx     Configuration index.
 * \param[in]  p_cfg       Pointer to ASE codec configuration: @ref T_AUDIO_ASE_CODEC_CFG.
 *
 * \return         The result of BAP unicast audio configuring ASE codec.
 * \retval true    Configuring ASE codec is successful.
 * \retval false   Configuring ASE codec failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_cfg_ase_codec(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                     T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                     uint8_t cfg_idx, T_AUDIO_ASE_CODEC_CFG *p_cfg);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio configure session QoS.
 *
 * This API is used when receive @ref AUDIO_GROUP_MSG_BAP_START_QOS_CFG.
 *
 * \param[in]  handle      Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  p_cfg       Pointer to QoS configuration: @ref T_AUDIO_SESSION_QOS_CFG.
 *
 * \return         The result of BAP unicast audio configuring session QoS.
 * \retval true    Configuring session QoS is successful.
 * \retval false   Configuring session QoS failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_cfg_session_qos(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                       T_AUDIO_SESSION_QOS_CFG *p_cfg);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio configure ASE QoS.
 *
 * This API is used when receive @ref AUDIO_GROUP_MSG_BAP_START_QOS_CFG.
 *
 * \param[in]  handle      Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  dev_handle  Device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \param[in]  ase_id      ASE id.
 * \param[in]  p_cfg       Pointer to ASE QoS configuration: @ref T_AUDIO_ASE_QOS_CFG.
 *
 * \return         The result of BAP unicast audio configure ASE QoS.
 * \retval true    BAP unicast audio configure ASE QoS is successful.
 * \retval false   BAP unicast audio configure ASE QoS failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_cfg_ase_qos(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                   T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                   uint8_t ase_id, T_AUDIO_ASE_QOS_CFG *p_cfg);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio get session QoS.
 *
 * \param[in]  handle      Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in,out]  p_cfg   Pointer to session QoS configuration: @ref T_AUDIO_SESSION_QOS_CFG.
 *
 * \return         The result of BAP unicast audio getting session QoS.
 * \retval true    Getting session QoS is successful.
 * \retval false   Getting session QoS failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_get_session_qos(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                       T_AUDIO_SESSION_QOS_CFG *p_cfg);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio get ASE QoS.
 *
 * \param[in]  handle      Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  dev_handle  Device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \param[in]  ase_id      ASE id.
 * \param[in,out]  p_cfg   Pointer to ASE QoS configuration: @ref T_AUDIO_ASE_QOS_CFG.
 *
 * \return         The result of BAP unicast audio getting ASE QoS.
 * \retval true    Getting ASE QoS is successful.
 * \retval false   Getting ASE QoS failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_get_ase_qos(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                   T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                   uint8_t ase_id, T_AUDIO_ASE_QOS_CFG *p_cfg);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio configure ASE metadata.
 *
 * This API is used when receiving @ref AUDIO_GROUP_MSG_BAP_START_METADATA_CFG.
 *
 * \param[in]  handle        Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  dev_handle    Device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \param[in]  ase_id        ASE id.
 * \param[in]  metadata_len  Metadata length.
 * \param[in]  p_metadata    Pointer to metadata.
 *
 * \return         The result of BAP unicast audio configuring ASE metadata.
 * \retval true    Configuring ASE metadata is successful.
 * \retval false   Configuring ASE metadata failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_cfg_ase_metadata(T_AUDIO_STREAM_SESSION_HANDLE handle,
                                        T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                        uint8_t ase_id, uint8_t metadata_len, uint8_t *p_metadata);

/**
 * bap_unicast_client.h
 *
 * \brief  Remove BAP unicast audio configuration.
 *
 * \param[in]  handle        Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 *
 * \return         The result of removing BAP unicast audio configuration.
 * \retval true    Removing BAP unicast audio configuration is successful.
 * \retval false   Removing BAP unicast audio configuration failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_remove_cfg(T_AUDIO_STREAM_SESSION_HANDLE handle);

/**
 * bap_unicast_client.h
 *
 * \brief  Remove BAP unicast audio session.
 *
 * \param[in]  handle        Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 *
 * \return         The result of removing BAP unicast audio session.
 * \retval true    Removing BAP unicast audio session is successful.
 * \retval false   Removing BAP unicast audio session failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_remove_session(T_AUDIO_STREAM_SESSION_HANDLE handle);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio start streaming.
 *
 * \param[in]  handle        Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 *
 * \return         The result of BAP unicast audio starting streaming.
 * \retval true    Starting streaming is successful.
 * \retval false   Starting streaming failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_start(T_AUDIO_STREAM_SESSION_HANDLE handle);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio stop streaming.
 *
 * \param[in]  handle          Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 * \param[in]  cis_timeout_ms  CIS timeout.
 *
 * \return         The result of BAP unicast audio stopping streaming.
 * \retval true    Stopping streaming is successful.
 * \retval false   Stopping streaming failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_stop(T_AUDIO_STREAM_SESSION_HANDLE handle, uint32_t cis_timeout_ms);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio action release.
 *
 * \param[in]  handle          Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 *
 * \return         The result of BAP unicast audio action release.
 * \retval true    BAP unicast audio action release is successful.
 * \retval false   BAP unicast audio action release failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_release(T_AUDIO_STREAM_SESSION_HANDLE handle);

/**
 * bap_unicast_client.h
 *
 * \brief  BAP unicast audio update metadata.
 *
 * \param[in]  handle          Audio streaming session handle: @ref T_AUDIO_STREAM_SESSION_HANDLE.
 *
 * \return         The result of BAP unicast audio updating metadata.
 * \retval true    BAP unicast audio updating metadata is successful.
 * \retval false   BAP unicast audio updating metadata failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool bap_unicast_audio_update(T_AUDIO_STREAM_SESSION_HANDLE handle);

/**
 * bap_unicast_client.h
 *
 * \brief  ASCS client get ASE data.
 *
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  char_instance_id ASE characteristic instance id.
 * \param[in,out]  p_ase_data   Pointer to ASE data: @ref T_ASE_CHAR_DATA.
 * \param[in]  direction        Direction: @ref T_AUDIO_DIRECTION.
 *
 * \return         The result of ASCS client getting ASE data.
 * \retval true    ASCS client getting ASE data is successful.
 * \retval false   ASCS client getting ASE data failed.
 *
 * \ingroup BAP_Unicast_Client_Exported_Functions
 */
bool ascs_client_get_ase_data(uint16_t conn_handle, uint8_t char_instance_id,
                              T_ASE_CHAR_DATA *p_ase_data,
                              uint8_t direction);
/**
 * End of BAP_Unicast_Client_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
