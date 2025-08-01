/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_msc.h"
#include "os_wrapper.h"
#if defined(CONFIG_AMEBASMART)
#include "ameba_sd.h"
#endif

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "MSC";
// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_MSC_USB_HOTPLUG					1
#define CONFIG_USBD_MSC_SD_HOTPLUG					0

#if USBD_MSC_RAM_DISK && (CONFIG_USBD_MSC_SD_HOTPLUG == 1)
#error "Use RAM as storage media, no hotplug"
#endif

#if !defined(CONFIG_AMEBASMART) && (CONFIG_USBD_MSC_SD_HOTPLUG == 1)
#error "SD hotplug is not supported"
#endif

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_MSC_SPEED						USB_SPEED_FULL
#else
#define CONFIG_USBD_MSC_SPEED						USB_SPEED_HIGH
#endif

// Thread priorities
#define CONFIG_USBD_MSC_INIT_THREAD_PRIORITY		5U
#define CONFIG_USBD_MSC_USB_HOTPLUG_THREAD_PRIORITY		8U // Should be higher than CONFIG_USBD_MSC_ISR_THREAD_PRIORITY

/* Private types -------------------------------------------------------------*/
typedef enum {
	USBD_MSC_HOTPLUG_NONE = 0U,
	USBD_MSC_USB_HOTPLUG,
	USBD_MSC_SD_HOTPLUG
} usbd_msc_hotplug_type_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void msc_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t msc_cfg = {
	.speed = CONFIG_USBD_MSC_SPEED,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART)
	.ext_intr_en = USBD_EPMIS_INTR,
	.nptx_max_epmis_cnt = 100U,
#elif defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 660U,
	.ptx_fifo_depth = {16U, 256U, 16U, 16U, 16U},
#elif defined (CONFIG_AMEBASMARTPLUS)
	.rx_fifo_depth = 914U,
	.ptx_fifo_depth = {256U, 16U, 16U, 16U, 16U},
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 677U,
	.ptx_fifo_depth = {256U, 16U, 16U, 16U},
#endif
};

static usbd_msc_cb_t msc_cb = {
	.status_changed = msc_cb_status_changed
};

#if CONFIG_USBD_MSC_USB_HOTPLUG
static u8 msc_usb_attach_status;
static rtos_sema_t msc_usb_status_changed_sema;
#endif

#if CONFIG_USBD_MSC_SD_HOTPLUG
static u8 msc_sd_status;
static rtos_sema_t msc_sd_status_changed_sema;
#endif
usbd_msc_hotplug_type_t msc_hotplug_ongoing_type;

/* Private functions ---------------------------------------------------------*/

static void msc_cb_status_changed(u8 old_status, u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USB status change: %d -> %d\n", old_status, status);
#if CONFIG_USBD_MSC_USB_HOTPLUG
	msc_usb_attach_status = status;
	if (msc_hotplug_ongoing_type != USBD_MSC_SD_HOTPLUG) {
		rtos_sema_give(msc_usb_status_changed_sema);
	}
#endif
}

#if CONFIG_USBD_MSC_USB_HOTPLUG
static void msc_usb_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(msc_usb_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (msc_usb_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				msc_hotplug_ongoing_type = USBD_MSC_USB_HOTPLUG;
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_msc_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				usbd_msc_disk_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				usbd_msc_disk_init();
				ret = usbd_init(&msc_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_msc_init(&msc_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
				msc_hotplug_ongoing_type = USBD_MSC_HOTPLUG_NONE;
			} else if (msc_usb_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_MSC_USB_HOTPLUG

#if CONFIG_USBD_MSC_SD_HOTPLUG
static void msc_sd_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(msc_sd_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (msc_sd_status == SD_NODISK) {
				msc_hotplug_ongoing_type = USBD_MSC_SD_HOTPLUG;
				RTK_LOGS(TAG, RTK_LOG_INFO, "SD card removed\n");
				usbd_msc_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "SD card insert, re-init USB\n");
				SD_CardInit();
				ret = usbd_init(&msc_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_msc_init(&msc_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
				msc_hotplug_ongoing_type = USBD_MSC_HOTPLUG_NONE;
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "SD card hotplug thread fail\n");
	rtos_task_delete(NULL);
}

static void sd_intr_cb(SD_RESULT res)
{

	RTK_LOGS(TAG, RTK_LOG_INFO, "SD callback status: %d\n", res);
	msc_sd_status = res;
	if (msc_hotplug_ongoing_type != USBD_MSC_USB_HOTPLUG) {
		rtos_sema_give(msc_sd_status_changed_sema);
	}
}
#endif // CONFIG_USBD_MSC_SD_HOTPLUG

static void example_usbd_msc_thread(void *param)
{
	int status = 0;
#if CONFIG_USBD_MSC_USB_HOTPLUG
	rtos_task_t usb_task;
#endif
#if CONFIG_USBD_MSC_SD_HOTPLUG
	rtos_task_t sd_task;
#endif
	UNUSED(param);

#if CONFIG_USBD_MSC_USB_HOTPLUG
	rtos_sema_create(&msc_usb_status_changed_sema, 0U, 1U);
#endif

#if CONFIG_USBD_MSC_SD_HOTPLUG
	rtos_sema_create(&msc_sd_status_changed_sema, 0U, 1U);
#endif

	status = usbd_msc_disk_init();
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init disk fail\n");
		goto exit_usbd_msc_disk_init_fail;
	}

	status = usbd_init(&msc_cfg);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBD fail\n");
		goto exit_usbd_init_fail;
	}

	status = usbd_msc_init(&msc_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init MSC class fail\n");
		goto exit_usbd_msc_init_fail;
	}

#if CONFIG_USBD_MSC_USB_HOTPLUG
	status = rtos_task_create(&usb_task, "msc_usb_hotplug_thread", msc_usb_hotplug_thread, NULL, 1024U, CONFIG_USBD_MSC_USB_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto exit_create_hotplug_fail;
	}
#endif // CONFIG_USBD_MSC_USB_HOTPLUG

#if CONFIG_USBD_MSC_SD_HOTPLUG
	SD_SetCdCallback(sd_intr_cb);
	status = rtos_task_create(&sd_task, "msc_sd_hotplug_thread", msc_sd_hotplug_thread, NULL, 1024U, CONFIG_USBD_MSC_USB_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create SD card hotplug thread fail\n");
		goto exit_create_msc_sd_hotplug_fail;
	}
#endif // CONFIG_USBD_MSC_SD_HOTPLUG

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD MSC demo start\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_MSC_SD_HOTPLUG
exit_create_msc_sd_hotplug_fail:
#if CONFIG_USBD_MSC_USB_HOTPLUG
	rtos_task_delete(usb_task);
#endif
#endif

exit_create_hotplug_fail:
	usbd_msc_deinit();

exit_usbd_msc_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	usbd_msc_disk_deinit();

exit_usbd_msc_disk_init_fail:
#if CONFIG_USBD_MSC_SD_HOTPLUG
	rtos_sema_delete(msc_sd_status_changed_sema);
#endif
#if CONFIG_USBD_MSC_USB_HOTPLUG
	rtos_sema_delete(msc_usb_status_changed_sema);
#endif

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_msc(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "example_usbd_msc_thread", example_usbd_msc_thread, NULL, 1024, CONFIG_USBD_MSC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD MSC thread fail\n");
	}
}

