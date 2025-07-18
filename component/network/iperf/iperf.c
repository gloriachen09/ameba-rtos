#include "lwip_netconf.h"

#if defined(CONFIG_WHC_HOST)
#define BSD_STACK_SIZE		    1024
#else
#define BSD_STACK_SIZE		    512
#endif
#define DEFAULT_PORT            5001
#define DEFAULT_TIME            10
#define SERVER_BUF_SIZE         1500
#define CLIENT_BUF_SIZE         1460
#define KB                      1024
#define MB                      1048576//1024*1024
#define DEFAULT_UDP_BANDWIDTH   16777216 // 16MByes => 16*1024*1024 Bytes = 128 Mbits
#define DEFAULT_REPORT_INTERVAL 0xffffffff
#define DEFAULT_UDP_TOS_VALUE   1 // AC_BE
#define SUBSTREAM_FLAG 0x0100

//Max number of streaming
#define MULTI_STREAM_NUM 10

#define printf		DiagPrintfNano

struct stream_id_t {
	uint8_t id_used;
	uint8_t  terminate;
};


struct iperf_data_t {
	uint64_t total_size;
	uint64_t bandwidth;
	rtos_task_t task;
	int  server_fd;
	int  client_fd;
	uint32_t buf_size;
	uint32_t time;
	uint32_t report_interval;
	uint16_t port;
	uint8_t  server_ip[16];
	uint8_t  mul_ip[16];
	uint8_t  role; // 'c' for client and 's' for server
	uint8_t  protocol; // 'u' for udp and 't' for tcp
	uint8_t  tos_value;
	int8_t   stream_id;
	//uint8_t  terminate;
	uint8_t  bidirection;
	uint16_t  is_sub_stream; //Byte[1] : this client stream is created by bidirection parameter, Byte[0] : main stream id
	struct iperf_data_t *prev;
	struct iperf_data_t *next;
};

struct iperf_tcp_client_hdr {
	uint32_t flags;
	uint32_t numThreads;
	uint32_t mPort;
	uint32_t bufferlen;
	uint32_t mWinband;
	uint32_t mAmount;
};

struct iperf_udp_datagram {
	uint32_t id;
	uint32_t tv_sec;
	uint32_t tv_usec;
};

struct iperf_udp_client_hdr {
	uint32_t id;
	uint32_t tv_sec;
	uint32_t tv_usec;
	uint32_t flags;
	uint32_t numThreads;
	uint32_t mPort;
	uint32_t bufferlen;
	uint32_t mWinband;
	uint32_t mAmount;
};

struct iperf_udp_server_hdr {
	uint32_t flags;
	uint32_t total_len1;
	uint32_t total_len2;
	uint32_t stop_sec;
	uint32_t stop_usec;
	uint32_t error_cnt;
	uint32_t outorder_cnt;
	uint32_t datagrams;
	uint32_t jitter1;
	uint32_t jitter2;
};

uint8_t g_is_iperf_init = 0;
struct stream_id_t g_stream_id[MULTI_STREAM_NUM];
struct iperf_data_t *stream_data_head = NULL;

rtos_mutex_t g_tptest_log_mutex = NULL;
rtos_mutex_t g_tptest_mutex = NULL;

#define tptest_res_log(...)       do { \
                                    if(NULL != g_tptest_log_mutex){\
                                        rtos_mutex_take(g_tptest_log_mutex, MUTEX_WAIT_TIMEOUT); \
                                        DiagPrintfNano(__VA_ARGS__); \
                                        rtos_mutex_give(g_tptest_log_mutex); \
                                    }\
                                    else{\
                                        DiagPrintfNano(__VA_ARGS__); \
                                    }\
                                }while(0)

static void iperf_test_handler(void *param);

void iperf_init(void)
{
	int i = 0;

	if (g_is_iperf_init == 0) {
		if (NULL == g_tptest_mutex) {
			rtos_mutex_create(&g_tptest_mutex);
		}
		if (NULL == g_tptest_log_mutex) {
			rtos_mutex_create(&g_tptest_log_mutex);
		}
		for (i = 0; i < MULTI_STREAM_NUM; i++) {
			g_stream_id[i].id_used = 0;
			g_stream_id[i].terminate = 0;
		}
		g_is_iperf_init = 1;
	}

	return;
}

void iperf_deinit(void)
{
	int i = 0;

	if (g_is_iperf_init == 1) {
		if (g_tptest_mutex) {
			rtos_mutex_delete(g_tptest_mutex);
			g_tptest_mutex = NULL;
		}
		if (g_tptest_log_mutex) {
			rtos_mutex_delete(g_tptest_log_mutex);
			g_tptest_log_mutex = NULL;
		}
		for (i = 0; i < MULTI_STREAM_NUM; i++) {
			g_stream_id[i].id_used = 0;
			g_stream_id[i].terminate = 0;
		}
		g_is_iperf_init = 0;
	}

	return;
}

// Note : Need to use mutex protect
// stream id range : 0 ~ MULTI_STREAM_NUM
struct iperf_data_t *init_stream_data(uint8_t protocol, uint8_t role)
{
	int8_t i = 0, stream_id = -1;
	struct iperf_data_t *stream_data = NULL;
	struct iperf_data_t *stream_data_tail = NULL;

	for (i = 0; i < MULTI_STREAM_NUM; i++) {
		if (g_stream_id[i].id_used == 0) {
			g_stream_id[i].id_used = 1;
			g_stream_id[i].terminate = 0;
			stream_id = i;
			break;
		}
	}

	if (stream_id == -1) {
		tptest_res_log("\n\r[ERROR] stream reach limit(%d)!\n\r", MULTI_STREAM_NUM);
		return NULL;
	}

	stream_data = rtos_mem_malloc(sizeof(struct iperf_data_t));
	if (!stream_data) {
		g_stream_id[stream_id].id_used = 0;
		tptest_res_log("\n\r[ERROR] stream_data allocate failed!\n\r");
		return NULL;
	}

	memset(stream_data, 0, sizeof(struct iperf_data_t));
	stream_data->stream_id = stream_id;
	stream_data->role = role;
	stream_data->protocol = protocol;
	stream_data->port = DEFAULT_PORT;
	stream_data->report_interval = DEFAULT_REPORT_INTERVAL;

	if (role == 'c') {
		stream_data->buf_size = CLIENT_BUF_SIZE;
		stream_data->time = DEFAULT_TIME;
	} else if (role == 's') {
		stream_data->buf_size = SERVER_BUF_SIZE;
	}

	if (protocol == 'u') {
		stream_data->bandwidth = DEFAULT_UDP_BANDWIDTH;
		stream_data->tos_value = DEFAULT_UDP_TOS_VALUE;
	}

	if (stream_data_head == NULL) {
		stream_data_head = stream_data;
		stream_data_head->prev = NULL;
		stream_data_head->next = NULL;
	} else {
		stream_data_tail = stream_data_head;
		while (stream_data_tail->next != NULL) {
			stream_data_tail = stream_data_tail->next;
		}
		stream_data_tail->next = stream_data;
		stream_data->prev = stream_data_tail;
		stream_data->next = NULL;
	}

	return stream_data;
}

// Note : Need to use mutex protect when calling the function
void free_stream_data(struct iperf_data_t *stream_data)
{
	if (stream_data) {

		if (stream_data == stream_data_head) {
			stream_data_head = stream_data_head->next;
			if (stream_data_head != NULL) {
				stream_data_head->prev = NULL;
			}
		} else if (stream_data->next != NULL) {
			(stream_data->next)->prev = stream_data->prev;
			(stream_data->prev)->next = stream_data->next;
		} else {
			(stream_data->prev)->next = NULL;
		}

		g_stream_id[stream_data->stream_id].id_used = 0;
		g_stream_id[stream_data->stream_id].terminate = 0;
		rtos_mem_free(stream_data);
		stream_data = NULL;
	}
	return;
}

uint64_t km_parser(char *buf, int len)
{
	uint64_t ret = 0;
	int keyword_num = 0, num_len = 0;
	char num_str[17] = "\0";
	uint64_t num;
	char unit = '\0';

	if (len > 16) {
		return ret;
	}

	while ((buf[keyword_num] != '\0') && (keyword_num < len)) {
		if (buf[keyword_num] >= '0' && buf[keyword_num] <= '9') {
		} else if ((unit == '\0') && (buf[keyword_num] == 'k' || buf[keyword_num] == 'K' ||
									  buf[keyword_num] == 'm' || buf[keyword_num] == 'M')) {
			unit = buf[keyword_num];
		} else {
			return 0;
		}
		keyword_num++;
	}

	num_len = (unit == '\0') ? keyword_num : keyword_num - 1;
	strncpy(num_str, buf, num_len);
	num_str[num_len] = '\0';

	char *endptr;
	num = strtoull(num_str, &endptr, 10);
	if (*endptr != '\0' || endptr == num_str) {
		return 0;
	}

	switch (unit) {
	case 'k':
	case 'K':
		ret = num * KB;
		break;
	case 'm':
	case 'M':
		ret = num * MB;
		break;
	default:
		ret = num;
		break;
	}

	return ret;
}

int tcp_client_func(struct iperf_data_t iperf_data)
{
	struct sockaddr_in  ser_addr;
	uint32_t            i = 0;
	uint32_t            start_time, end_time, bandwidth_time, report_start_time;
	uint64_t            total_size = 0, bandwidth_size = 0, report_size = 0;
	struct iperf_tcp_client_hdr client_hdr;
	char *tcp_client_buffer = NULL;

	tcp_client_buffer = rtos_mem_malloc(iperf_data.buf_size);
	if (!tcp_client_buffer) {
		tptest_res_log("\n\r[ERROR] %s: Alloc buffer failed\n\r", __func__);
		goto exit2;
	}

	//filling the buffer
	for (i = 0; i < iperf_data.buf_size; i++) {
		tcp_client_buffer[i] = (char)(i % 10);
	}

	//create socket
	if ((iperf_data.client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		tptest_res_log("\n\r[ERROR] %s: Create TCP socket failed\n\r", __func__);
		goto exit2;
	}

	//initialize value in dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(iperf_data.port);
	ser_addr.sin_addr.s_addr = inet_addr((char const *)iperf_data.server_ip);

	tptest_res_log("%s: Server IP=%s, port=%d\n\r", __func__, iperf_data.server_ip, iperf_data.port);
	tptest_res_log("%s: Create socket fd = %d\n\r", __func__, iperf_data.client_fd);

	//Connecting to server
	if (connect(iperf_data.client_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
		tptest_res_log("\n\r[ERROR] %s: Connect to server failed\n\r", __func__);
		goto exit1;
	}
	tptest_res_log("%s: Connect to server successfully\n\r", __func__);

	// For "iperf -d" command, send first packet with iperf client header
	if (iperf_data.bidirection) {
		client_hdr.flags = 0x01000080;
		client_hdr.numThreads = 0x01000000;
		client_hdr.mPort = htonl(iperf_data.port);
		client_hdr.bufferlen = 0;
		client_hdr.mWinband = 0;
		client_hdr.mAmount = htonl(~(iperf_data.time * 100) + 1);
		if (send(iperf_data.client_fd, (char *) &client_hdr, sizeof(client_hdr), 0) <= 0) {
			tptest_res_log("\n\r[ERROR] %s: TCP client send data error\n\r", __func__);
			goto exit1;
		}
	}

	if (iperf_data.total_size == 0) {
		start_time = rtos_time_get_current_system_time_ms();
		end_time = start_time;
		bandwidth_time = start_time;
		report_start_time = start_time;
		while (((end_time - start_time) <= (1000 * iperf_data.time)) && (!g_stream_id[iperf_data.stream_id].terminate)) {
			if (send(iperf_data.client_fd, tcp_client_buffer, iperf_data.buf_size, 0) <= 0) {
				tptest_res_log("\n\r[ERROR] %s: TCP client send data error\n\r", __func__);
				goto exit1;
			}
			total_size += iperf_data.buf_size;
			bandwidth_size += iperf_data.buf_size;
			report_size += iperf_data.buf_size;
			end_time = rtos_time_get_current_system_time_ms();

			if ((iperf_data.bandwidth != 0) && (bandwidth_size >= iperf_data.bandwidth) && ((end_time - bandwidth_time) < (1000 * 1))) {
				rtos_time_delay_ms(1000 * 1 - (end_time - bandwidth_time));
				end_time = rtos_time_get_current_system_time_ms();
				bandwidth_time = end_time;
				bandwidth_size = 0;
			}

			if ((iperf_data.report_interval != DEFAULT_REPORT_INTERVAL) && ((end_time - report_start_time) >= (1000 * iperf_data.report_interval))) {
				tptest_res_log("tcp_c: id[%d] Send %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data.stream_id, (int)(report_size / KB), (int)(end_time - report_start_time),
							   (int)((report_size * 8) / (end_time - report_start_time)));
				report_start_time = end_time;
				bandwidth_time = end_time;
				report_size = 0;
				bandwidth_size = 0;
			}
		}
	} else {
		start_time = rtos_time_get_current_system_time_ms();
		end_time = start_time;
		bandwidth_time = start_time;
		report_start_time = start_time;
		while ((total_size < iperf_data.total_size) && (!g_stream_id[iperf_data.stream_id].terminate)) {
			if (send(iperf_data.client_fd, tcp_client_buffer, iperf_data.buf_size, 0) <= 0) {
				tptest_res_log("\n\r[ERROR] %s: TCP client send data error\n\r", __func__);
				goto exit1;
			}
			total_size += iperf_data.buf_size;
			bandwidth_size += iperf_data.buf_size;
			report_size += iperf_data.buf_size;
			end_time = rtos_time_get_current_system_time_ms();

			if ((iperf_data.bandwidth != 0) && (bandwidth_size >= iperf_data.bandwidth) && ((end_time - bandwidth_time) < (1000 * 1))) {
				rtos_time_delay_ms(1000 * 1 - (end_time - bandwidth_time));
				end_time = rtos_time_get_current_system_time_ms();
				bandwidth_time = end_time;
				bandwidth_size = 0;
			}

			if ((iperf_data.report_interval != DEFAULT_REPORT_INTERVAL) && ((end_time - report_start_time) >= (1000 * iperf_data.report_interval))) {
				tptest_res_log("tcp_c: id[%d] Send %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data.stream_id, (int)(report_size / KB), (int)(end_time - report_start_time),
							   (int)((report_size * 8) / (end_time - report_start_time)));
				report_start_time = end_time;
				bandwidth_time = end_time;
				report_size = 0;
				bandwidth_size = 0;
			}
		}
	}

	if (g_stream_id[iperf_data.stream_id].terminate) {
		tptest_res_log("TCP Client terminated\n\r");
	}

	if (iperf_data.is_sub_stream & SUBSTREAM_FLAG) {
		//This stream is created by bidirectional parameter
		tptest_res_log("tcp_c: [END] id[%d] Bidirection Totally send %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data.is_sub_stream & 0xff, (int)(total_size / KB),
					   (int)(end_time - start_time),
					   (int)((total_size * 8) / (end_time - start_time)));
	} else {
		tptest_res_log("tcp_c: [END] id[%d] Totally send %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data.stream_id, (int)(total_size / KB),
					   (int)(end_time - start_time),
					   (int)((total_size * 8) / (end_time - start_time)));
	}

exit1:
	closesocket(iperf_data.client_fd);
exit2:
	tptest_res_log("%s: Close client socket\n\r", __func__);
	if (tcp_client_buffer) {
		rtos_mem_free(tcp_client_buffer);
		tcp_client_buffer = NULL;
	}

	return 0;
}

int tcp_server_func(struct iperf_data_t iperf_data)
{
	struct sockaddr_in   ser_addr, client_addr;
	int                  addrlen = sizeof(struct sockaddr_in);
	int                  n = 1;
	int                  recv_size = 0;
	uint64_t             total_size = 0, report_size = 0;
	uint32_t             start_time, report_start_time, end_time;
	struct iperf_tcp_client_hdr client_hdr;
	char *tcp_server_buffer = NULL;
	struct iperf_data_t *tcp_client_data = NULL;
	int socket_connect = 0;
	fd_set read_fds;
	struct timeval select_timeout;
	int frame_num = 0;

	tcp_server_buffer = rtos_mem_malloc(iperf_data.buf_size);
	if (!tcp_server_buffer) {
		tptest_res_log("\n\r[ERROR] %s: Alloc buffer failed\n\r", __func__);
		goto exit3;
	}

	//create socket
	if ((iperf_data.server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		tptest_res_log("\n\r[ERROR] %s: Create socket failed\n\r", __func__);
		goto exit3;
	}

	tptest_res_log("%s: Create socket fd = %d\n\r", __func__, iperf_data.server_fd);

	setsockopt(iperf_data.server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &n, sizeof(n));

	//initialize structure dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(iperf_data.port);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// binding the TCP socket to the TCP server address
	if (bind(iperf_data.server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
		tptest_res_log("\n\r[ERROR] %s: Bind socket failed\n\r", __func__);
		goto exit2;
	}
	tptest_res_log("%s: Bind socket successfully\n\r", __func__);

	//Make it listen to socket with max 20 connections
	if (listen(iperf_data.server_fd, 20) != 0) {
		tptest_res_log("\n\r[ERROR] %s: Listen socket failed\n\r", __func__);
		goto exit2;
	}
	tptest_res_log("%s: Listen port %d\n\r", __func__, iperf_data.port);

	select_timeout.tv_sec = 0;
	select_timeout.tv_usec = 500000; //500ms

	while (!socket_connect && !g_stream_id[iperf_data.stream_id].terminate) {
		FD_ZERO(&read_fds);
		FD_SET(iperf_data.server_fd, &read_fds);
		if (select(iperf_data.server_fd + 1, &read_fds, NULL, NULL, &select_timeout)) {
			if (FD_ISSET(iperf_data.server_fd, &read_fds)) {
				if ((iperf_data.client_fd = accept(iperf_data.server_fd, (struct sockaddr *)&client_addr, (u32_t *)&addrlen)) < 0) {
					tptest_res_log("\n\r[ERROR] %s: Accept TCP client socket error!\n\r", __func__);
					goto exit2;
				}
				socket_connect = 1;
				tptest_res_log("%s: Accept connection successfully\n\r", __func__);
			}
		}
	}
	if (g_stream_id[iperf_data.stream_id].terminate) {
		goto exit2;
	}

	recv_size = recv(iperf_data.client_fd, tcp_server_buffer, iperf_data.buf_size, 0);
	if (!iperf_data.bidirection) { //Server
		//parser the amount of tcp iperf setting
		memcpy(&client_hdr, tcp_server_buffer, sizeof(client_hdr));
		if (ntohl(client_hdr.flags) == 0x80000001) { //bi-direction, create client to send packets back
			rtos_mutex_take(g_tptest_mutex, MUTEX_WAIT_TIMEOUT);
			tcp_client_data = init_stream_data('t', 'c');
			rtos_mutex_give(g_tptest_mutex);
			if (tcp_client_data == NULL) {
				tptest_res_log("\n\r[ERROR] init_stream_data failed!\n\r");
				goto exit1;
			}
			if (client_hdr.mAmount != 0) {
				client_hdr.mAmount = ntohl(client_hdr.mAmount);
				if (client_hdr.mAmount > 0x7fffffff) {
					client_hdr.mAmount = (~(client_hdr.mAmount) + 1) / 100;
					tcp_client_data->time = client_hdr.mAmount;
				} else {
					tcp_client_data->total_size = client_hdr.mAmount;
				}
			}
			strncpy((char *)tcp_client_data->server_ip, inet_ntoa(client_addr.sin_addr), (strlen(inet_ntoa(client_addr.sin_addr))));
			tcp_client_data->port = ntohl(client_hdr.mPort);
			tcp_client_data->buf_size = CLIENT_BUF_SIZE;
			tcp_client_data->report_interval = DEFAULT_REPORT_INTERVAL;
			tcp_client_data->is_sub_stream = SUBSTREAM_FLAG | iperf_data.stream_id;

			if (rtos_task_create(&tcp_client_data->task, "iperf_test_handler", iperf_test_handler, (void *) tcp_client_data, BSD_STACK_SIZE * 4,
								 1 + 4) != RTK_SUCCESS) {
				tptest_res_log("\n\rTCP ERROR: Create TCP client task failed.\n\r");
				rtos_mutex_take(g_tptest_mutex, MUTEX_WAIT_TIMEOUT);
				free_stream_data(tcp_client_data);
				rtos_mutex_give(g_tptest_mutex);
				goto exit1;
			}
		}
	}

	start_time = rtos_time_get_current_system_time_ms();
	end_time = start_time;
	report_start_time = start_time;
	while (!g_stream_id[iperf_data.stream_id].terminate) {
		recv_size = recv(iperf_data.client_fd, tcp_server_buffer, iperf_data.buf_size, 0);  //MSG_DONTWAIT   MSG_WAITALL
		if (recv_size < 0) {
			tptest_res_log("\n\r[ERROR] %s: Receive data failed\n\r", __func__);
			goto exit1;
		} else if (recv_size == 0) {
			//tptest_res_log("%s: [END] Totally receive %d KBytes in %d ms, %d Kbits/sec\n\r",__func__, (uint32_t) (total_size/KB),(uint32_t) (end_time-start_time),((uint32_t) (total_size*8)/(end_time - start_time)));
			//total_size=0;
			//close(iperf_data.client_fd);
			//goto Restart;
			break;
		}
		end_time = rtos_time_get_current_system_time_ms();
		total_size += recv_size;
		report_size += recv_size;
		frame_num++;
		if ((iperf_data.report_interval != DEFAULT_REPORT_INTERVAL) && ((end_time - report_start_time) >= (1000 * iperf_data.report_interval))) {
			tptest_res_log("tcp_s: id[%d] Receive %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data.stream_id, (int)(report_size / KB), (int)(end_time - report_start_time),
						   (int)((report_size * 8) / (end_time - report_start_time)));
			report_start_time = end_time;
			report_size = 0;
		}
	}

exit1:
	if (total_size != 0) {
		tptest_res_log("tcp_s: [END] id[%d] Totally receive %d KBytes in %d ms, frame_num = %d, %d Kbits/sec\n\r", iperf_data.stream_id, (int)(total_size / KB),
					   (int)(end_time - start_time), frame_num, (int)((uint64_t)(total_size * 8) / (end_time - start_time)));
	}

	// close the connected socket after receiving from connected TCP client
	close(iperf_data.client_fd);

exit2:
	// close the listening socket
	close(iperf_data.server_fd);
	if (g_stream_id[iperf_data.stream_id].terminate) {
		tptest_res_log("TCP Server terminated\n\r");
	}

exit3:
	if (tcp_server_buffer) {
		rtos_mem_free(tcp_server_buffer);
		tcp_server_buffer = NULL;
	}
	return 0;
}


SRAM_WLAN_CRITICAL_CODE_SECTION
uint32_t send_udp_packets(struct iperf_data_t *iperf_data, struct iperf_udp_client_hdr *client_hdr, char *udp_client_buffer, struct sockaddr_in *ser_addr)
{
	int addrlen = sizeof(struct sockaddr_in);
	uint32_t end_time, start_time, bandwidth_time, report_start_time;
	u32_t now;
	uint32_t id_cnt = 0;
	uint64_t total_size = 0, bandwidth_size = 0, report_size = 0;

	start_time = rtos_time_get_current_system_time_ms();
	end_time = start_time;
	bandwidth_time = start_time;
	report_start_time = start_time;

	while ((!g_stream_id[iperf_data->stream_id].terminate) &&
		   ((iperf_data->total_size == 0 && (end_time - start_time <= 1000 * iperf_data->time)) ||
			(iperf_data->total_size != 0 && total_size < iperf_data->total_size))) {

		now = rtos_time_get_current_system_time_ms();
		client_hdr->id = htonl(id_cnt);
		client_hdr->tv_sec  = htonl(now / 1000);
		client_hdr->tv_usec = htonl((now % 1000) * 1000);
		memcpy(udp_client_buffer, client_hdr, sizeof(*client_hdr));

		if (sendto(iperf_data->client_fd, udp_client_buffer, iperf_data->buf_size, 0, (struct sockaddr *)ser_addr, addrlen) < 0) {
			if (iperf_data->total_size == 0) {
				//Add delay to avoid consuming too much CPU when data link layer is busy
				rtos_time_delay_ms(2);
			} else {
				//tptest_res_log("[ERROR] %s: UDP client send data error\n\r",__func__);
			}
		} else {
			total_size += iperf_data->buf_size;
			bandwidth_size += iperf_data->buf_size;
			report_size += iperf_data->buf_size;
			id_cnt++;
		}
		end_time = rtos_time_get_current_system_time_ms();

		if ((bandwidth_size >= iperf_data->bandwidth) && ((end_time - bandwidth_time) < (1000 * 1))) {
			rtos_time_delay_ms(1000 * 1 - (end_time - bandwidth_time));
			end_time = rtos_time_get_current_system_time_ms();
			bandwidth_time = end_time;
			bandwidth_size = 0;
		}

		if ((iperf_data->report_interval != DEFAULT_REPORT_INTERVAL) && ((end_time - report_start_time) >= (1000 * iperf_data->report_interval))) {
			tptest_res_log("udp_c: id[%d] Send %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data->stream_id, (int)(report_size / KB), (int)(end_time - report_start_time),
						   (int)((report_size * 8) / (end_time - report_start_time)));
			report_start_time = end_time;
			bandwidth_time = end_time;
			report_size = 0;
			bandwidth_size = 0;
		}
	}

	if (g_stream_id[iperf_data->stream_id].terminate) {
		tptest_res_log("UDP Client terminated\n\r");
	}

	if (iperf_data->is_sub_stream & SUBSTREAM_FLAG) {
		//This stream is created by bidirectional parameter
		tptest_res_log("udp_c: [END] id[%d] Bidirection Totally send %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data->is_sub_stream & 0xff, (int)(total_size / KB),
					   (int)(end_time - start_time),
					   (int)((total_size * 8) / (end_time - start_time)));
	} else {
		tptest_res_log("udp_c: [END] id[%d] Totally send %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data->stream_id, (int)(total_size / KB),
					   (int)(end_time - start_time),
					   (int)((total_size * 8) / (end_time - start_time)));
	}
	return id_cnt;
}

int udp_client_func(struct iperf_data_t iperf_data)
{
	struct sockaddr_in  ser_addr;
	uint32_t			i = 0;
	int                 addrlen = sizeof(struct sockaddr_in);
	struct iperf_udp_client_hdr client_hdr = {0};
	u32_t now;
	uint32_t id_cnt = 0;
	int tos_value = (int)iperf_data.tos_value;// fix optlen check fail issue in lwip_setsockopt_impl
	char *udp_client_buffer = NULL;

	udp_client_buffer = rtos_mem_malloc(iperf_data.buf_size);
	if (!udp_client_buffer) {
		tptest_res_log("\n\r[ERROR] %s: Alloc buffer failed\n\r", __func__);
		goto exit2;
	}

	//filling the buffer
	for (i = 0; i < iperf_data.buf_size; i++) {
		udp_client_buffer[i] = (char)(i % 10);
	}

	//create socket
	if ((iperf_data.client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		tptest_res_log("\n\r[ERROR] %s: Create UDP socket failed\n\r", __func__);
		goto exit2;
	}

	//initialize value in dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(iperf_data.port);
	ser_addr.sin_addr.s_addr = inet_addr((char const *)iperf_data.server_ip);

	tptest_res_log("%s: Server IP=%s, port=%d\n\r", __func__, iperf_data.server_ip, iperf_data.port);
	tptest_res_log("%s: Create socket fd = %d\n\r", __func__, iperf_data.client_fd);

	if (setsockopt(iperf_data.client_fd, IPPROTO_IP, IP_TOS, &tos_value, sizeof(tos_value)) != 0) {
		tptest_res_log("\n\r[ERROR] %s: Set sockopt failed\n\r", __func__);
		goto exit1;
	}

	client_hdr.numThreads = htonl(0x00000001);
	client_hdr.mPort = htonl(iperf_data.port);
	client_hdr.bufferlen = 0;
	client_hdr.mWinband = htonl(iperf_data.bandwidth);

	if (iperf_data.bidirection) {
		client_hdr.id = 0;
		client_hdr.tv_sec = 0;
		client_hdr.tv_usec = 0;
		client_hdr.flags = htonl(0x80000001);
		client_hdr.mAmount = htonl(~(iperf_data.time * 100) + 1);
		memcpy(udp_client_buffer, &client_hdr, sizeof(client_hdr));
	}
	if (iperf_data.total_size == 0) {
		client_hdr.mAmount = htonl(~(iperf_data.time * 100) + 1);
	} else {
		client_hdr.mAmount = htonl(iperf_data.total_size);
	}

	id_cnt = send_udp_packets(&iperf_data, &client_hdr, udp_client_buffer, &ser_addr);
	// send a final terminating datagram
	i = 0;
	int rc;
	fd_set readSet;
	struct timeval timeout;
	uint32_t stop_ms;
	uint64_t total_len;

	now = rtos_time_get_current_system_time_ms();
	client_hdr.id = htonl(-id_cnt);
	client_hdr.tv_sec  = htonl(now / 1000);
	client_hdr.tv_usec = htonl((now % 1000) * 1000);
	memcpy(udp_client_buffer, &client_hdr, sizeof(client_hdr));

	while (i < 10) {
		i++;
		sendto(iperf_data.client_fd, udp_client_buffer, iperf_data.buf_size, 0, (struct sockaddr *)&ser_addr, addrlen);

		// wait until the socket is readable, or our timeout expires
		FD_ZERO(&readSet);
		FD_SET(iperf_data.client_fd, &readSet);
		timeout.tv_sec  = 0;
		timeout.tv_usec = 250000; // quarter second, 250 ms
		rc = select(iperf_data.client_fd + 1, &readSet, NULL, NULL, &timeout);
		if (rc == -1) {
			break;
		}

		if (rc == 0) {
			// select timed out
			continue;
		} else {
			// socket ready to read
			rc = read(iperf_data.client_fd, udp_client_buffer, iperf_data.buf_size);
			if (rc < 0) {
				break;
			}
			if (rc >= (int)(sizeof(struct iperf_udp_datagram) + sizeof(struct iperf_udp_server_hdr))) {
				struct iperf_udp_datagram *UDP_Hdr;
				struct iperf_udp_server_hdr *hdr;

				UDP_Hdr = (struct iperf_udp_datagram *) udp_client_buffer;
				hdr = (struct iperf_udp_server_hdr *)(UDP_Hdr + 1);
				tptest_res_log("%s: Server Report\n\r", __func__);
				if ((ntohl(hdr->flags) & 0x80000000) != 0) {
					stop_ms = ntohl(hdr->stop_sec) * 1000 + ntohl(hdr->stop_usec) / 1000;
					total_len = (((uint64_t) ntohl(hdr->total_len1)) << 32) + ntohl(hdr->total_len2);
					tptest_res_log("udp_c: [END] id[%d] Totally send %d KBytes in %d ms, %d Kbits/sec\n\r", iperf_data.stream_id, (int)(total_len / KB), (int)stop_ms,
								   (int)(total_len * 8 / stop_ms));
				}
			}
			break;
		}
	}
exit1:
	close(iperf_data.client_fd);
exit2:
	tptest_res_log("%s: Close client socket\n\r", __func__);
	if (udp_client_buffer) {
		rtos_mem_free(udp_client_buffer);
		udp_client_buffer = NULL;
	}
	return 0;
}

SRAM_WLAN_CRITICAL_CODE_SECTION
void recv_udp_packets(struct iperf_data_t *iperf_data, int first_packet_size, uint8_t boundary_type, uint32_t client_amount, char *udp_server_buffer)
{
	struct sockaddr_in client_addr;
	int addrlen = sizeof(struct sockaddr_in);
	int recv_size = 0;
	int datagram_id;
	int frame_num = 0;
	uint64_t total_size = first_packet_size, report_size = first_packet_size;
	uint32_t start_time, report_start_time, end_time;

	start_time = rtos_time_get_current_system_time_ms();
	report_start_time = start_time;
	end_time = start_time;
	while (!g_stream_id[iperf_data->stream_id].terminate &&
		   ((boundary_type == 0) ||
			((boundary_type == 1) && (total_size < client_amount)) || // size_boundary
			((boundary_type == 2) && (end_time - start_time <= 1000 * client_amount)))) { // time_boundary

		recv_size = recvfrom(iperf_data->server_fd, udp_server_buffer, iperf_data->buf_size, 0, (struct sockaddr *)&client_addr, (u32_t *)&addrlen);

		if (recv_size < 0) {
			if (boundary_type) {
				tptest_res_log("\n\r[ERROR] %s: Receive data failed\n\r", __func__);
			} else {
				tptest_res_log("%s: Receive data timeout\n\r", __func__);
			}
			goto exit1;
		} else if ((recv_size > 0) && (iperf_data->bidirection) && (!boundary_type)) {
			sendto(iperf_data->server_fd, udp_server_buffer, recv_size, 0, (struct sockaddr *) &client_addr, (u32_t)addrlen);
		}
		// ack data to client
		// Not send ack to prevent send fail due to limited skb, but it will have warning at iperf client
		//sendto(server_fd,udp_server_buffer,ret,0,(struct sockaddr*)&client_addr,sizeof(client_addr));
		datagram_id = ntohl(((struct iperf_udp_datagram *)udp_server_buffer)->id);
		if (datagram_id < 0) {
			sendto(iperf_data->server_fd, udp_server_buffer, 0, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
			g_stream_id[iperf_data->stream_id].terminate = 1;
		}

		end_time = rtos_time_get_current_system_time_ms();
		total_size += recv_size;
		report_size += recv_size;
		frame_num++;

		if ((iperf_data->report_interval != DEFAULT_REPORT_INTERVAL) && ((end_time - report_start_time) >= (1000 * iperf_data->report_interval))) {
			tptest_res_log("udp_s: id[%d] Receive %d KBytes in %d ms, %d Kbits/sec\n\r",
						   iperf_data->stream_id, (int)(report_size / KB),
						   (int)(end_time - report_start_time),
						   (int)((report_size * 8) / (end_time - report_start_time)));
			report_start_time = end_time;
			report_size = 0;
		}
	}
exit1:
	if (total_size != 0) {
		tptest_res_log("udp_s: [END] id[%d] Totally receive %d KBytes in %d ms, frame_num = %d, %d Kbits/sec\n\r",
					   iperf_data->stream_id, (int)(total_size / KB),
					   (int)(end_time - start_time), frame_num,
					   (int)((uint64_t)(total_size * 8) / (end_time - start_time)));
	}
}

int udp_server_func(struct iperf_data_t iperf_data)
{
	ip_mreq mreq;
	struct sockaddr_in   ser_addr, client_addr;
	int                  addrlen = sizeof(struct sockaddr_in);
	int                  n = 1;
	int                  first_packet_size = 0;
	struct iperf_udp_client_hdr client_hdr;
	uint8_t boundary_type = 0;
	char *udp_server_buffer = NULL;
	int socket_connect = 0;
	fd_set read_fds;
	struct timeval select_timeout;
	int recv_timeout = 1000;
	struct timeval timeout;

	udp_server_buffer = rtos_mem_malloc(iperf_data.buf_size);
	if (!udp_server_buffer) {
		tptest_res_log("\n\r[ERROR] %s: Alloc buffer failed\n\r", __func__);
		goto exit2;
	}

	//create socket
	if ((iperf_data.server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		tptest_res_log("\n\r[ERROR] %s: Create socket failed\n\r", __func__);
		goto exit2;
	}
	tptest_res_log("%s: Create socket fd = %d, port = %d\n\r", __func__, iperf_data.server_fd, iperf_data.port);

	setsockopt(iperf_data.server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &n, sizeof(n));

	if (strcmp((char const *)iperf_data.mul_ip, "\0") != 0) {
		//initialize multi_addr join request
		memset(&mreq, 0, sizeof(ip_mreq));
		mreq.imr_multiaddr.s_addr = inet_addr((char const *)iperf_data.mul_ip);
		mreq.imr_interface.s_addr =  htonl(INADDR_ANY);

		if (setsockopt(iperf_data.server_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != 0) {
			tptest_res_log("\n\r[ERROR] %s:  IGMP join failed\n\r", __func__);
			goto exit1;
		}
	}

	//initialize structure dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(iperf_data.port);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// binding the UDP socket to the UDP server address
	if (bind(iperf_data.server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
		tptest_res_log("\n\r[ERROR] %s: Bind socket failed\n\r", __func__);
		goto exit1;
	}

	tptest_res_log("%s: Bind socket successfully\n\r", __func__);

	select_timeout.tv_sec = 0;
	select_timeout.tv_usec = 500000; //500ms

	while (!socket_connect && !g_stream_id[iperf_data.stream_id].terminate) {
		FD_ZERO(&read_fds);
		FD_SET(iperf_data.server_fd, &read_fds);
		if (select(iperf_data.server_fd + 1, &read_fds, NULL, NULL, &select_timeout)) {
			if (FD_ISSET(iperf_data.server_fd, &read_fds)) {
				//wait for first packet to start
				first_packet_size = recvfrom(iperf_data.server_fd, udp_server_buffer, iperf_data.buf_size, 0, (struct sockaddr *) &client_addr, (u32_t *)&addrlen);
				socket_connect = 1;
			}
		}
	}
	if (g_stream_id[iperf_data.stream_id].terminate) {
		goto exit1;
	}

	timeout.tv_sec  = recv_timeout / 1000;
	timeout.tv_usec = (recv_timeout % 1000) * 1000;
	setsockopt(iperf_data.server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	if (iperf_data.total_size) {
		client_hdr.mAmount = iperf_data.total_size;
		boundary_type = 1; //size_boundary
	} else if (iperf_data.time) {
		client_hdr.mAmount = iperf_data.time;
		boundary_type = 2; //time_boundary
	}
	recv_udp_packets(&iperf_data, first_packet_size, boundary_type, client_hdr.mAmount, udp_server_buffer);

exit1:
	// close the listening socket
	close(iperf_data.server_fd);
	if (g_stream_id[iperf_data.stream_id].terminate) {
		tptest_res_log("UDP Server terminated\n\r");
	}

exit2:
	if (udp_server_buffer) {
		rtos_mem_free(udp_server_buffer);
		udp_server_buffer = NULL;
	}
	return 0;
}

static void iperf_test_handler(void *param)
{
	/* To avoid gcc warnings */
	struct iperf_data_t *stream_data = (struct iperf_data_t *) param;
	int i = 0, deinit_iperf = 1;

	rtos_time_delay_ms(100);
	if (stream_data->protocol == 'u') {
		if (stream_data->role == 'c') {
			tptest_res_log("Start UDP client! id = [%d]\n\r", stream_data->stream_id);
			udp_client_func(stream_data[0]);
			tptest_res_log("UDP client stopped!\n\r");
		} else if (stream_data->role == 's') {
			tptest_res_log("Start UDP server! id = [%d]\n\r", stream_data->stream_id);
			udp_server_func(stream_data[0]);
			tptest_res_log("UDP server stopped!\n\r");
		}
	} else if (stream_data->protocol == 't') {
		if (stream_data->role == 'c') {
			tptest_res_log("Start TCP client! id = [%d]\n\r", stream_data->stream_id);
			tcp_client_func(stream_data[0]);
			tptest_res_log("TCP client stopped!\n\r");
		} else if (stream_data->role == 's') {
			tptest_res_log("Start TCP server! id = [%d]\n\r", stream_data->stream_id);
			tcp_server_func(stream_data[0]);
			tptest_res_log("TCP server stopped!\n\r");
		}
	} else {
		tptest_res_log("\n\r[ERROR] Wrong data\n\r");
	}

	rtos_mutex_take(g_tptest_mutex, MUTEX_WAIT_TIMEOUT);
	free_stream_data(stream_data);
	rtos_mutex_give(g_tptest_mutex);

#if defined(INCLUDE_uxTaskGetStackHighWaterMark) && (INCLUDE_uxTaskGetStackHighWaterMark == 1)
	tptest_res_log("Min available stack size of %s = %d * %d bytes\n\r", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL), sizeof(portBASE_TYPE));
#endif

	//If all stream finish, deinit iperf
	for (i = 0; i < MULTI_STREAM_NUM; i++) {
		if (g_stream_id[i].id_used != 0) {
			deinit_iperf = 0;
			break;
		}
	}
	if (deinit_iperf) {
		iperf_deinit();
	}

	rtos_task_delete(NULL);
}

int cmd_iperf(int argc, char **argv)
{
	int argv_count = 2;
	uint8_t stream_id;
	struct iperf_data_t *stream_data = NULL;
	struct iperf_data_t *stream_data_s = NULL;
	struct iperf_data_t *stream_data_list = NULL;
	uint8_t protocol = 0;
	int i = 0;
	int error_no = 0;
	char *endptr = NULL;
	int temp = -1;

	iperf_init();
	rtos_mutex_take(g_tptest_mutex, MUTEX_WAIT_TIMEOUT);

	if (argc < 2) {
		error_no = 3;
		goto exit;
	}

	if (strncmp(argv[0], "tcp", 3) == 0) {
		protocol = 't';
	} else if (strncmp(argv[0], "udp", 3) == 0) {
		protocol = 'u';
	} else {
		error_no = 3;
		goto exit;
	}

	while (argv_count <= argc) {
		//first operation
		if (argv_count == 2) {
			if (strcmp(argv[argv_count - 1], "-s") == 0) {
				stream_data = init_stream_data(protocol, 's');
				if (stream_data == NULL) {
					tptest_res_log("\n\r[ERROR] init_stream_data failed!\n\r");
					error_no = 4;
					goto exit;
				}
				argv_count++;
			} else if (strcmp(argv[argv_count - 1], "stop") == 0) {
				if (argc == 3) {
					temp = strtol(argv[2], &endptr, 10);
					if (*endptr != '\0' || endptr == argv[2] || temp < 0 || temp >= MULTI_STREAM_NUM) {
						error_no = 3;
						goto exit;
					}
					stream_id = (uint8_t)temp;
					g_stream_id[stream_id].terminate = 1;
					rtos_mutex_give(g_tptest_mutex);
					return error_no;
				} else if (argc == 2) {
					for (i = 0; i < MULTI_STREAM_NUM; i++) {
						if (g_stream_id[i].id_used) {
							g_stream_id[i].terminate = 1;
						}
					}
					rtos_mutex_give(g_tptest_mutex);
					return error_no;
				} else {
					error_no = 3;
					goto exit;
				}
			} else if (strcmp(argv[argv_count - 1], "-c") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				stream_data = init_stream_data(protocol, 'c');
				if (stream_data == NULL) {
					tptest_res_log("\n\r[ERROR] init_stream_data failed!\n\r");
					error_no = 4;
					goto exit;
				}
				strncpy((char *)stream_data->server_ip, argv[2], sizeof(stream_data->server_ip) - 1);
				stream_data->server_ip[sizeof(stream_data->server_ip) - 1] = '\0';
				argv_count += 2;
			} else if (strcmp(argv[argv_count - 1], "?") == 0) {
				if (argc == 2) {
					stream_data_list = stream_data_head;
					while (stream_data_list != NULL) {
						tptest_res_log("[%d] %s_%s, port=%d\n\r", stream_data_list->stream_id, (stream_data_list->protocol == 't' ? "tcp" : "udp"),
									   (stream_data_list->role == 'c' ? "client" : "server"), stream_data_list->port);
						stream_data_list = stream_data_list->next;
					}
					rtos_mutex_give(g_tptest_mutex);
					return error_no;
				} else {
					error_no = 3;
					goto exit;
				}
			} else {
				error_no = 3;
				goto exit;
			}
		} else {
			if ((strcmp(argv[argv_count - 1], "-b") == 0)) {
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				if (stream_data->role == 'c') {
					stream_data->bandwidth = km_parser(argv[argv_count], strlen(argv[argv_count]));
					if (stream_data->bandwidth == 0) {
						error_no = 3;
						goto exit;
					}
					stream_data->bandwidth = stream_data->bandwidth / 8; //bits to Bytes
				} else {
					error_no = 3;
					goto exit;
				}
				argv_count += 2;
			} else if ((strcmp(argv[argv_count - 1], "-d") == 0)) {
				stream_data->bidirection = 1;
				argv_count += 1;
			} else if (strcmp(argv[argv_count - 1], "-i") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				temp = strtol(argv[argv_count], &endptr, 10);
				if (*endptr != '\0' || endptr == argv[argv_count] || temp <= 0) {
					error_no = 3;
					goto exit;
				}
				stream_data->report_interval = (uint32_t)temp;
				argv_count += 2;
			} else if (strcmp(argv[argv_count - 1], "-l") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				temp = strtol(argv[argv_count], &endptr, 10);
				if (*endptr != '\0' || endptr == argv[argv_count] || temp <= 0) {
					error_no = 3;
					goto exit;
				}
				stream_data->buf_size = (uint32_t)temp;
				argv_count += 2;
			} else if (strcmp(argv[argv_count - 1], "-n") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				stream_data->time = 0;
				stream_data->total_size = km_parser(argv[argv_count], strlen(argv[argv_count]));
				if (stream_data->total_size == 0) {
					error_no = 3;
					goto exit;
				}
				argv_count += 2;
			} else if (strcmp(argv[argv_count - 1], "-p") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				temp = strtol(argv[argv_count], &endptr, 10);
				if (*endptr != '\0' || endptr == argv[argv_count] || temp < 1 || temp > 65535) {
					error_no = 3;
					goto exit;
				}
				stream_data->port = (uint16_t)temp;
				argv_count += 2;
			}
#ifdef CONFIG_WLAN
			else if (strcmp(argv[argv_count - 1], "-S") == 0) { //for wmm test
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				if (stream_data->role == 'c') {
					if (atoi(argv[argv_count]) >= 0 && atoi(argv[argv_count]) <= 255) {
						stream_data->tos_value = (uint8_t) atoi(argv[argv_count]);
					} else {
						error_no = 3;
						goto exit;
					}
				} else {
					error_no = 3;
					goto exit;
				}
				argv_count += 2;
			}
#endif
			else if (strcmp(argv[argv_count - 1], "-t") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				temp = strtol(argv[argv_count], &endptr, 10);
				if (*endptr != '\0' || endptr == argv[argv_count] || temp <= 0) {
					error_no = 3;
					goto exit;
				}
				stream_data->time = (uint32_t)temp;
				argv_count += 2;
			} else if (strcmp(argv[argv_count - 1], "-B") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 3;
					goto exit;
				}
				strncpy((char *)stream_data->mul_ip, argv[argv_count], sizeof(stream_data->mul_ip) - 1);
				stream_data->mul_ip[sizeof(stream_data->mul_ip) - 1] = '\0';
				argv_count += 2;
			} else {
				error_no = 3;
				goto exit;
			}
		}
	}

	if (stream_data->role == 's') {
		if (rtos_task_create(&stream_data->task, "iperf_test_handler", iperf_test_handler, (void *) stream_data, BSD_STACK_SIZE * 4,
							 2 + 4) != RTK_SUCCESS) {
			tptest_res_log("UDP ERROR: Create UDP server task failed.\n\r");
			goto exit;
		}
	} else if (stream_data->role == 'c') {
		if (stream_data->bidirection == 1) {
			stream_data_s = init_stream_data(protocol, 's');
			if (stream_data_s == NULL) {
				tptest_res_log("[ERROR] init_stream_data failed!\n\r");
				goto exit;
			}
			if (rtos_task_create(&stream_data_s->task, "iperf_test_handler", iperf_test_handler, (void *) stream_data_s, BSD_STACK_SIZE * 4,
								 2 + 4) != RTK_SUCCESS) {
				tptest_res_log("UDP ERROR: Create UDP server task failed.\n\r");
				goto exit;
			}
		}
		if (rtos_task_create(&stream_data->task, "iperf_test_handler", iperf_test_handler, (void *) stream_data, BSD_STACK_SIZE * 4,
							 1 + 4) != RTK_SUCCESS) {
			tptest_res_log("UDP ERROR: Create UDP client task failed.\n\r");
			if (stream_data_s != NULL) {
				rtos_task_delete(stream_data_s->task);
			}
			goto exit;
		}
	}

	rtos_mutex_give(g_tptest_mutex);
	return error_no;

exit:
	free_stream_data(stream_data);
	free_stream_data(stream_data_s);
	rtos_mutex_give(g_tptest_mutex);

	return error_no;
}

