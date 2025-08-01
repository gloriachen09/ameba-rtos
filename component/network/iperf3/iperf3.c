/*
 * iperf, Copyright (c) 2014, 2015, 2017, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */
#include "iperf_config.h"
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <getopt.h>
#include <errno.h>

#include "lwip_netconf.h" //realtek add
#include "rtw_misc.h"

#include "iperf.h"
#include "iperf_api.h"
#include "units.h"
#include "iperf_locale.h"
#include "net.h"
#include "timer.h"
#include "diag.h"

struct iperf_test *test;
struct task_struct g_server_task;
struct task_struct g_client_task;
unsigned char g_server_terminate = 0;
unsigned char g_client_terminate = 0;

#define printf	DiagPrintfNano

void server_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
	rtos_time_delay_ms(500);
	printf("server test start\n");
	g_server_terminate = 0;
	int rc;
	while (!g_server_terminate) {
		rc = iperf_run_server(test);
		if (rc < 0) {
			iperf_err(test, "error - %s\n", iperf_strerror(i_errno));
			if (rc < -1) {
				iperf_err(test, "exiting\n");
			}
		}

		iperf_reset_test(test);
		if (iperf_get_test_one_off(test)) {
			break;
		}
		rtos_time_delay_ms(5);
	}

	iperf_free_test(test);
	tmr_destroy();
	printf("server test end\n");
	g_server_task.task = 0;
	rtos_task_delete(g_server_task.task);
}

void client_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
	g_client_terminate = 0;
	rtos_time_delay_ms(500);
	printf("client test start\n");

	if (iperf_run_client(test) < 0) {
		iperf_err(test, "error - %s", iperf_strerror(i_errno));
	}

	iperf_free_test(test);
	tmr_destroy();
	printf("client test end\n");
	g_client_task.task = 0;
	rtos_task_delete(g_client_task.task);
}

/**************************************************************************/
extern void lwip_setsockrcvevent(int fd, int rcvevent);
extern void lwip_selectevindicate(int fd);
static void indicate_server(void)
{
	if (test->listener >= 0) {
		lwip_setsockrcvevent(test->listener, 1);
		lwip_selectevindicate(test->listener);
	}

	if (test->ctrl_sck >= 0) {
		lwip_setsockrcvevent(test->ctrl_sck, 1);
		lwip_selectevindicate(test->ctrl_sck);
	}

}
/**************************************************************************/


int cmd_iperf3(int argc, char **argv)
{
	int error_no = 0;

	if (strcmp(argv[1], "stop") == 0) {
		switch (test->role) {
		case 's':
			g_server_terminate = 1;
			printf("\n\r start terminate server test:%d\n", g_server_terminate);
			if (g_server_task.task) {
				indicate_server();
				rtos_time_delay_ms(1000);
			}
			break;
		case 'c':
			g_client_terminate = 1;
			printf("\n\r start terminate client test:%d\n", g_client_terminate);
			if (g_client_task.task) {
				rtos_time_delay_ms(1000);
			}
			break;
		}
		goto Exit;
	}

	if (g_server_task.task != NULL && test->role == 's') {
		printf("\n\rserver test is already running.\n");
		goto Exit;
	}
	if (g_client_task.task != NULL && test->role == 'c') {
		printf("\n\rclient test is already running.\n");
		goto Exit;
	}

	test = iperf_new_test();
	if (!test) {
		iperf_err(NULL, "create new test error - %s", iperf_strerror(i_errno));
		goto Exit;
	}

	iperf_defaults(test);	/* sets defaults */
	if (iperf_parse_arguments(test, argc, argv) < 0) {
		iperf_err(test, "parameter error - %s", iperf_strerror(i_errno));
		printf("\n");
		usage_long(stdout);
		iperf_free_test(test);
		error_no = 2;
		i_errno = IENONE;
		goto Exit;
	}

	switch (test->role) {
	case 's':
		if (rtos_task_create(&g_server_task.task, ((const char *)"server_thread"), server_thread, NULL, 4096, 2 + 4) != RTK_SUCCESS) {
			printf("\n\r%s rtos_task_create(server_thread) failed", __FUNCTION__);
			iperf_free_test(test);
		}
		break;
	case 'c':
		if (rtos_task_create(&g_client_task.task, ((const char *)"client_thread"), client_thread, NULL, 4096, 1 + 4) != RTK_SUCCESS) {
			printf("\n\r%s rtos_task_create(client_thread) failed", __FUNCTION__);
			iperf_free_test(test);
		}
		break;
	default:
		usage();
		break;
	}
Exit:
	return error_no;
}
