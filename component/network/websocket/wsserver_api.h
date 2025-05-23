#ifndef _WS_SERVER_API_H_
#define _WS_SERVER_API_H_

#include "os_wrapper.h"
#include "wifi_api.h"

/********************Define the secure level***************************/
#define WS_SERVER_SECURE_NONE        0   /*!< Running with WS server */
#define WS_SERVER_SECURE_TLS         1   /*!< Running with WSS server */
#define WS_SERVER_SECURE_TLS_VERIFY  2   /*!< Running with WSS server and verify client */
/**********************************************************************/


/*******************Define the debug message level*********************/
#define WS_SERVER_DEBUG_OFF          0   /*!< Disable websocket server debug log */
#define WS_SERVER_DEBUG_ON           1   /*!< Enable websocket server debug log */
#define WS_SERVER_DEBUG_VERBOSE      2   /*!< Enable websocket server verbose debug log */


extern uint8_t ws_server_debug;
#define ws_server_log(...) \
	do { \
		if(ws_server_debug) { \
			rtos_critical_enter(RTOS_CRITICAL_NETWORK); \
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[WS_SERVER] "); \
			RTK_LOGS(NOTAG, RTK_LOG_INFO, __VA_ARGS__); \
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r"); \
			rtos_critical_exit(RTOS_CRITICAL_NETWORK); \
		} \
	} while(0)

#define ws_server_log_verbose(...) \
	do { \
		if(ws_server_debug == WS_SERVER_DEBUG_VERBOSE) { \
			rtos_critical_enter(RTOS_CRITICAL_NETWORK); \
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[WS_SERVER] "); \
			RTK_LOGS(NOTAG, RTK_LOG_INFO, __VA_ARGS__); \
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r"); \
			rtos_critical_exit(RTOS_CRITICAL_NETWORK); \
		} \
	} while(0)

/***************************************************************************/

/**********************Define the data structures***************************/

/**
  * @brief	The enum is the list of client connection status.
  */

typedef enum {
	WSS_CLOSED = 0,		/*!< Client Connection closed */
	WSS_CONNECTING,	/*!< Client is connecting */
	WSS_CONNECTED1,	/*!< Client is connected */
	WSS_CONNECTED2,	/*!< Connected and server sent ping to client */
	WSS_CLOSING,	/*!< Client will be closed */
} ws_conn_state;


/**
  * @brief  The structure is the context used for websocket server handshakes header parsing.
  * @note   Only header string includes string terminator.
  */
struct ws_request {
	char *header;				/*!< handshake header string parsed in websocket handshake */
	size_t header_len;				/*!< handshake header string length */
	char *path;					/*!< Pointer to resource path in the parsed handshake header string */
	size_t path_len;				/*!< Resource path data length */
	char *query;					/*!< Pointer to query string in the parsed handshake header string */
	size_t query_len;				/*!< Query string data length */
	char *version;				/*!< Pointer to HTTP version in the parsed handshake header string */
	size_t version_len;				/*!< HTTP version data length */
	uint8_t *host;					/*!< Pointer to Host header field in the parsed handshake header string */
	size_t host_len;				/*!< Host header field data length */
	char *ws_key;				/*!< Pointer to Sec-WebSocket-Key field in the parsed handshake header string */
	size_t ws_key_len;				/*!< Sec-WebSocket-Key field data length */
	char *ws_version;			/*!< Pointer to Sec-WebSocket-Versions field in the parsed handshake header string */
	size_t ws_version_len;				/*!< Sec-WebSocket-Key field data length */
};

/**
  * @brief  The structure is the context used for client connection.
  */
typedef struct _ws_conn {
	int sock;						/*!< Client socket descriptor for connection */
	struct ws_request request;		/*!< Context for websocket server request */
	void *tls;						/*!< Context for TLS connection */
	char *response_header;		/*!< Pointer to transmission buffer of ws handshake response header */
	uint32_t last_ping_sent_time;	/*!< Last ping sent time in system ticks */
	uint32_t last_data_comm_time;	/*!< Last data received or sent time in system ticks */
	int tx_len;						/*!< The length of the transmission data */
	int rx_len;						/*!< The length of the received data */
	uint8_t *txbuf;					/*!< Pointer to transmission buffer of ws server will send */
	uint8_t *rxbuf;					/*!< Pointer to receiving buffer which received from client */
	uint8_t *receivedData;			/*!< Pointer to decoded receiving data which received from client */
	ws_conn_state state;		/*!< Connection state */
	struct task_struct task;        /*!< Connection task context */
	int close_reason;			/*!< Connection close reason */
} ws_conn;

/**
  * @brief  The structure is the context used for websocket opcode.
  */
enum opcode_type {
	CONTINUATION = 0x0,
	TEXT_FRAME = 0x1,
	BINARY_FRAME = 0x2,
	CLOSE = 8,
	PING = 9,
	PONG = 0xa,
};

/**
  * @brief  The list of connection close reason
  */
#define WSS_CREATE_CONNECTION_FAIL	1 		/*!< Create connection fail*/
#define WSS_CONNECTION_INIT_FAIL	2 		/*!< Connection init fail*/
#define WSS_RECEIVE_CLIENT_CLOSE	3 		/*!< Receive close from client*/
#define WSS_SERVER_SEND_CLOSE	4 			/*!< Server send close to client*/
#define WSS_HANDSHAKE_FAIL	5 				/*!< Server handshake with client fail*/
#define WSS_HANDSHAKE_SELECT_TIMEOUT	6 	/*!< Select timeout when handshake*/
#define WSS_HANDSHAKE_FD_ISSET_FAIL	7 		/*!< FD is not set when handshake*/
#define WSS_TLS_HANDSHAKE_FAIL	8 			/*!< Server handshake with client fail when use tls*/
#define WSS_SET_NONBLOCK_FAIL	9 			/*!< Set nonblock fail*/
#define WSS_READ_DATA_FAIL	10 				/*!< Server read data fail*/
#define WSS_SEND_DATA_FAIL	11 				/*!< Server send data fail*/
#define WSS_SET_SOCKET_OPTION_FAIL	12 		/*!< Set socket option fail*/
#define WSS_NOT_GET_PONG	13 				/*!< Get pong timeout */
#define WSS_IDLE_TIMEOUT	14 				/*!< Idle timeout*/

/***************************************************************************/

/******************Functions of the websocekt server************************/

/**
 * @brief     This function is used to start an WS or WSS server.
 * @param[in] port: service port
 * @param[in] max_conn: max client connections allowed, maximum value will be limited by MEMP_NUM_NETCONN in lwipopts.h
 * @param[in] stack_bytes: thread stack size in bytes
 * @param[in] secure: security mode for WS or WSS. Must be WS_SERVER_SECURE_NONE, WS_SERVER_SECURE_TLS, WS_SERVER_SECURE_TLS_VERIFY.
 * @return		0 : if successful
 * @return		-1 : if error occurred
 */
int ws_server_start(uint16_t port, uint8_t max_conn, uint32_t stack_bytes, uint8_t secure);

/**
 * @brief     This function is used to stop a running server
 * @return    None
 */
void ws_server_stop(void);

/**
 * @brief	  This function is the callback function when getting message from connections.
 * @param[in] callback: function that resolve the message received with the opcode type.
 * @return	  None
 */
void ws_server_dispatch(void (*callback)(ws_conn *, int, enum opcode_type)) ;

/**
 * @brief     This function is used to setup websocket server debug.
 * @param[in] debug: flag to enable/disable ws_server debug. Must be WS_SERVER_DEBUG_OFF, WS_SERVER_DEBUG_ON, WS_SERVER_DEBUG_VERBOSE.
 * @return    None
 */
void ws_server_setup_debug(uint8_t debug);

/**
 * @brief     This function is used to setup certificate and key for server before starting with WSS.
 * @param[in] server_cert: string of server certificate
 * @param[in] server_key: string of server private key
 * @param[in] ca_certs: string including certificates in CA trusted chain
 * @return    0 : if successful
 * @return    -1 : if error occurred
 * @note      Must be used before ws_server_start() if staring WSS server
 */
int ws_server_setup_cert(const char *server_cert, const char *server_key, const char *ca_certs);

/**
 * @brief     This function is used to setup the interval of ping for server.
 * @param[in] interval_ms: interval in ms
 * @return    None
 * @note      The default value is 30s
 */
void ws_server_setup_ping_interval(uint32_t interval_ms);

/**
 * @brief     This function is used to setup the timeout if there is no data between server and client.
 * @param[in] timeout_ms: timeout in ms
 * @return    None
 * @note      The default value is 0 which means disable kick client even there is no data transmission.
 */
void ws_server_setup_idle_timeout(uint32_t timeout_ms);


/**
 * @brief     This function is used to setup the tx rx buffer size for each connection.
 * @param[in] tx_size: tx buffer size
 * @param[in] rx_size: rx_buffer
 * @return    None
 * @note      The default value is 256 bytes
 */
void ws_server_setup_tx_rx_size(size_t tx_size, size_t rx_size);

/**
 * @brief     This function is show the current connections and their status.
 * @return    None
 */
void ws_server_print_status(void);

/**
 * @brief     This function is used to get connection info.
 * @param[in] conn_no: the number of connection
 * @return    the ws_conn which contains the detail info of the connection
 * @note      The conn_no should be small than ws_server_max_conn
 */
ws_conn *ws_server_get_conn_info(int conn_no);

/**
 * @brief	  This function is used to sending Ping to websocket connection.
 * @param[in] ws_conn: the websocket connection
 * @return    None
 */
void ws_server_sendPing(ws_conn *conn);

/**
 * @brief	  This function is used to create the sending binary data and copy to tx_bufs. Data will be sent while polling.
 * @param[in] message: the binary data that will be sent to client
 * @param[in] message_len: the length of the binary data
 * @param[in] use_mask: 0/1; 1 means using mask for data
 * @param[in] ws_conn: the websocket connection
 * @return    None
 */
void ws_server_sendBinary(uint8_t *message, size_t message_len, int use_mask, ws_conn *conn);

/**
 * @brief	  This function is used to create the sending text data and copy to tx_bufs. Data will be sent while polling.
 * @param[in] message: the text data that will be sent to client
 * @param[in] message_len: the length of the text data
 * @param[in] use_mask: 0/1; 1 means using mask for data
 * @param[in] ws_conn: the websocket connection
 * @return    None
 */
void ws_server_sendText(char *message, size_t message_len, int use_mask, ws_conn *conn);

/**
 * @brief	  This function is used to create the sending binary data and send directly instead of sending while polling.
 * @param[in] message: the binary data that will be sent to client
 * @param[in] message_len: the length of the binary data
 * @param[in] use_mask: 0/1; 1 means using mask for data
 * @param[in] ws_conn: the websocket connection
 * @return    None
 */
void ws_server_direct_sendBinary(uint8_t *message, size_t message_len, int use_mask, ws_conn *conn);

/**
 * @brief	  This function is used to create the sending text data and send directly instead of sending while polling.
 * @param[in] message: the text data that will be sent to client
 * @param[in] message_len: the length of the text data
 * @param[in] use_mask: 0/1; 1 means using mask for data
 * @param[in] ws_conn: the websocket connection
 * @return    None
 */
void ws_server_direct_sendText(char *message, size_t message_len, int use_mask, ws_conn *conn);


/**
 * @brief	  This function is used to sending close to websocket connection.
 * @param[in] ws_conn: the websocket connection
 * @return    None
 */
void ws_server_sendClose(ws_conn *conn);

/**
 * @brief	  This function is used to remove the websocket client connection.
 * @param[in] ws_conn: the websocket connection which will be removed
 * @return    None
 */
void ws_server_conn_remove(ws_conn *conn);
/***************************************************************************/

/**
 * @brief     This function is used to setup the poll timeout of the websocket client connection.
 * @param[in] timeout_sec: timeout in seconds
 * @return    None
 * @note      The default value is 1s.
 */
void ws_server_setup_poll_timeout(uint32_t timeout_sec);

/**
 * @brief	  This function is the callback function when a client connected.
 * @param[in] callback: function that indicate the connect event.
 * @return	  None
 */
void ws_server_dispatch_connect(void (*callback)(ws_conn *)) ;

/**
 * @brief	  This function is the callback function when a client disconnected.
 * @param[in] callback: function that indicate the disconnect event.
 * @return	  None
 */
void ws_server_dispatch_disconnect(void (*callback)(ws_conn *)) ;

/**
 * @brief	  This function is used to set close reason of the websocket client connection.
 * @param[in] ws_conn: the websocket connection
 * @param[in] reason: the close reason
 * @return    0 : if successful
 * @return    -1 : if error occurred
 */
void ws_server_set_close_reason(ws_conn *conn, int reason) ;

/**
 * @brief	  This function is used to get close reason of the websocket client connection.
 * @param[in] ws_conn: the websocket connection
 * @return    reason : if successful
 */
int ws_server_get_close_reason(ws_conn *conn) ;

#endif /* _WS_SERVER_API_H_ */
