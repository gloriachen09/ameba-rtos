#include "lwip_netconf.h"
#include "log.h"

#define CONNECT_REMOTE  0

#if CONNECT_REMOTE
#define REMOTE_HOST     "192.168.13.14"
#define REMOTE_PORT     5000
#endif
#define MAX_SOCKETS     10
#define SELECT_TIMEOUT  10
#define SERVER_PORT     5000
#define LISTEN_QLEN     2

static void example_socket_select_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	int max_socket_fd = -1;
#if CONNECT_REMOTE
	struct sockaddr_in remote_addr;
	int remote_fd = -1;
#endif
	struct sockaddr_in server_addr;
	int server_fd = -1;
	int socket_used[MAX_SOCKETS];

	// Delay to check successful WiFi connection and obtain of an IP address
	LwIP_Check_Connectivity();

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n====================Example: socket select====================\r\n");

	memset(socket_used, 0, sizeof(socket_used));

#if CONNECT_REMOTE
reconnect:
	if ((remote_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		remote_addr.sin_family = AF_INET;
		remote_addr.sin_addr.s_addr = inet_addr(REMOTE_HOST);
		remote_addr.sin_port = htons(REMOTE_PORT);

		if (connect(remote_fd, (struct sockaddr *) &remote_addr, sizeof(remote_addr)) == 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "connect socket fd(%d)\n", remote_fd);
			socket_used[remote_fd] = 1;

			if (remote_fd > max_socket_fd) {
				max_socket_fd = remote_fd;
			}
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "connect error\n");
			close(remote_fd);
			goto reconnect;
		}
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "socket error\n");
		goto exit;
	}
#endif
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
		server_addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "bind error\n");
			goto exit;
		}

		if (listen(server_fd, LISTEN_QLEN) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "listen error\n");
			goto exit;
		}

		socket_used[server_fd] = 1;

		if (server_fd > max_socket_fd) {
			max_socket_fd = server_fd;
		}
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "socket error\n");
		goto exit;
	}

	while (1) {
		int socket_fd;
		char buf[512] = {0};
		fd_set read_fds;
		struct timeval timeout;

		FD_ZERO(&read_fds);
		timeout.tv_sec = SELECT_TIMEOUT;
		timeout.tv_usec = 0;

		for (socket_fd = 0; socket_fd < MAX_SOCKETS; socket_fd ++)
			if (socket_used[socket_fd]) {
				FD_SET(socket_fd, &read_fds);
			}

		if (select(max_socket_fd + 1, &read_fds, NULL, NULL, &timeout)) {
			for (socket_fd = 0; socket_fd < MAX_SOCKETS; socket_fd ++) {
				if (socket_used[socket_fd] && FD_ISSET(socket_fd, &read_fds)) {
					if (socket_fd == server_fd) {
						struct sockaddr_in client_addr;
						unsigned int client_addr_size = sizeof(client_addr);
						int fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);

						if (fd >= 0) {
							RTK_LOGS(NOTAG, RTK_LOG_INFO, "accept socket fd(%d)\n", fd);
							socket_used[fd] = 1;

							if (fd > max_socket_fd) {
								max_socket_fd = fd;
							}
						} else {
							RTK_LOGS(NOTAG, RTK_LOG_ERROR, "accept error\n");
						}
					} else {
						int read_size = recv(socket_fd, buf, sizeof(buf), MSG_DONTWAIT);
						RTK_LOGS(NOTAG, RTK_LOG_INFO, "recv data=%s(len=%d) from socket_fd(%d)\n", buf, read_size, socket_fd);

						if (read_size > 0) {
							send(socket_fd, buf, (read_size > 512) ? 512 : read_size, MSG_DONTWAIT);
							RTK_LOGS(NOTAG, RTK_LOG_INFO, "send data=%s(len=%d) from socket_fd(%d)\n", buf, (read_size > 512) ? 512 : read_size, socket_fd);
						} else {
							RTK_LOGS(NOTAG, RTK_LOG_INFO, "socket fd(%d) disconnected\n", socket_fd);
							socket_used[socket_fd] = 0;
							close(socket_fd);
						}
					}
				}
			}
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "TCP server: no data in %d seconds\n", SELECT_TIMEOUT);
		}

		rtos_time_delay_ms(10);
	}

exit:
	if (server_fd >= 0) {
		close(server_fd);
	}

	rtos_task_delete(NULL);
}

void example_socket_select(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_socket_select_thread"), example_socket_select_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_socket_select_thread) failed", __FUNCTION__);
	}
}
