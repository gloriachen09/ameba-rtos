/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "example_gpio_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

void raw_gpio_port_demo(void)
{
	/* Enable GPIO function and clock */
	// RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	//int pin_index;
	int i;

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Bit\t");
	for (i = 0; i < 32; i++) {
		if (pin_mask & (0x1UL << i)) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%d ", i);
		}
	}
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "enabled!\n");

#if PORT_OUTPUT_TEST

	GPIO_PortDirection(GPIO_TEST_PORT, pin_mask, GPIO_Mode_OUT);

	while (1) {
		for (i = 0; i < LED_PATTERN_NUM; i++) {
			GPIO_PortWrite(GPIO_TEST_PORT, pin_mask, led_pattern[i]);
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%d]set port: 0x%x\r\n", i, pin_mask & led_pattern[i]);
			DelayMs(1000);
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%d]get port: 0x%x\r\n\n", i, GPIO_PortRead(GPIO_TEST_PORT, pin_mask));
		}

	}
#else
	int value_new, value_tmp, value_old;
	int stable;

	GPIO_PortDirection(GPIO_TEST_PORT, pin_mask, GPIO_Mode_IN);

	value_old = GPIO_PortRead(GPIO_TEST_PORT, pin_mask);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " > value_old: 0x%x\r\n", value_old);
	while (1) {
		// De-bounce
		value_new = GPIO_PortRead(GPIO_TEST_PORT, pin_mask);

		stable = 0;
		while (stable < 3) {
			value_tmp = GPIO_PortRead(GPIO_TEST_PORT, pin_mask);
			if (value_new != value_tmp) {
				value_new = value_tmp;
				stable = 0;
			} else {
				stable++;
			}
		}

		if (value_old != value_new) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " < 0x%x\r\n", value_new);
			value_old = value_new;
		}
		DelayMs(50);
	}
#endif
}

int example_raw_gpio_port(void)
{
	if (RTK_SUCCESS != rtos_task_create(NULL, "RAW_GPIO_PORT_TASK", (rtos_task_t)raw_gpio_port_demo, (void *)NULL, 3072, 1)) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Create RAW_GPIO_PORT_TASK Err!!!\n");
	}

	// rtos_sched_start();
	return 0;
}
