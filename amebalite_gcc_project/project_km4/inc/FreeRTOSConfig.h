/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
	See http://www.freertos.org/a00110.html for an explanation of the
	definitions contained in this file.
******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 * http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#if defined(__GNUC__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif
#ifndef __IASMARM__
#define __IASMARM__ 0 /* IAR */
#endif
#include "platform_autoconf.h"
#include "ameba_userheapcfg.h"

/* Realtek heap trace configuration. */
#ifdef CONFIG_HEAP_TRACE
#define traceMALLOC( pvAddress, uiSize ) trace_malloc(pvAddress,uiSize)
#define traceFREE( pvAddress, uiSize ) trace_free(pvAddress,uiSize)
#endif

/* Realtek back trace configuration. */
#ifdef CONFIG_DEBUG_BACK_TRACE
extern void TaskExitError(void);
#define configTASK_RETURN_ADDRESS TaskExitError
#endif

/* Realtek Heap Integrity Check configuration. */
#ifdef CONFIG_HEAP_INTEGRITY_CHECK_IN_TASK_SWITCHED_OUT
extern uint32_t ulPortCheckHeapIntegrity(int COMPREHENSIVE_CHECK);
#define traceTASK_SWITCHED_OUT ulPortCheckHeapIntegrity
#endif

/* Cortex M33 port configuration. */
#define configENABLE_MPU								0
#define portUSING_MPU_WRAPPERS							0
#define portRISCV_STACK_PROTECT							0
#define configENABLE_FPU								1

/* Configure secure image(RDP) exists or not. Set configENABLE_TRUSTZONE to 1 if enable RDP. */
#ifdef CONFIG_TRUSTZONE
#define configRUN_FREERTOS_SECURE_ONLY					0
#define configENABLE_TRUSTZONE							1
#else
#define configRUN_FREERTOS_SECURE_ONLY					1
#define configENABLE_TRUSTZONE							0
#endif

#define configSUPPORT_STATIC_ALLOCATION					1


/* Constants related to the behaviour or the scheduler. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION			0
#define configUSE_PREEMPTION							1
#define configUSE_TIME_SLICING							1
#define configMAX_PRIORITIES							( 11 )
#define configIDLE_SHOULD_YIELD							0
#define configUSE_16_BIT_TICKS							0 /* Only for 8 and 16-bit hardware. */

/* Constants that describe the hardware and memory usage. */
#define configCPU_CLOCK_HZ								SystemCoreClock
#define configMINIMAL_STACK_SIZE						( ( unsigned short ) 512 )
#define configMINIMAL_SECURE_STACK_SIZE					( 1024 )
#define configMAX_TASK_NAME_LEN							( 24 )

#define secureconfigTOTAL_SRAM_HEAP_SIZE			( ( ( size_t ) ( 6 * 1024 ) ) )
#define secureconfigTOTAL_PSRAM_HEAP_SIZE			( ( ( size_t ) ( 128 * 1024 ) ) )

/* Constants that build features in or out. */
#define configUSE_MUTEXES								1
#define configUSE_APPLICATION_TASK_TAG					1
#define configUSE_NEWLIB_REENTRANT						1
#define configUSE_CO_ROUTINES							1 ///
#define configMAX_CO_ROUTINE_PRIORITIES 				( 2 )
#define configUSE_COUNTING_SEMAPHORES					1
#define configUSE_RECURSIVE_MUTEXES						1
#define configUSE_QUEUE_SETS							1
#define configUSE_TASK_NOTIFICATIONS					1
#define configUSE_TRACE_FACILITY						1
#define configSUPPORT_STATIC_ALLOCATION					1
#define configUSE_POSIX_ERRNO						1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS			2

/* Constants that define which hook (callback) functions should be used. */
#define configUSE_IDLE_HOOK								1
#define configUSE_TICK_HOOK								0
#define configUSE_MALLOC_FAILED_HOOK					1

/* Constants provided for debugging and optimisation assistance. */
#define configCHECK_FOR_STACK_OVERFLOW					2
#define configQUEUE_REGISTRY_SIZE						0
#define configRECORD_STACK_HIGH_ADDRESS					1

/* Software timer definitions. */
#define configUSE_TIMERS								1
#define configTIMER_TASK_PRIORITY						 (configMAX_PRIORITIES - 1)
#if defined(CONFIG_WHC_DEV) || defined(CONFIG_WHC_NONE)
#define configTIMER_QUEUE_LENGTH					(10 + 64)
#else
#define configTIMER_QUEUE_LENGTH					10
#endif
#define configTIMER_TASK_STACK_DEPTH					( 512  )

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function.  NOTE:  Setting an INCLUDE_ parameter to 0 is
 * only necessary if the linker does not automatically remove functions that are
 * not referenced anyway. */
#define INCLUDE_vTaskPrioritySet						1
#define INCLUDE_uxTaskPriorityGet						1
#define INCLUDE_vTaskDelete								1
#define INCLUDE_vTaskCleanUpResources					0
#define INCLUDE_vTaskSuspend							1
#define INCLUDE_vTaskDelayUntil							1
#define INCLUDE_vTaskDelay								1
#define INCLUDE_pcTaskGetTaskName       				1
#define INCLUDE_uxTaskGetStackHighWaterMark				0
#define INCLUDE_xTaskGetIdleTaskHandle					0
#define INCLUDE_eTaskGetState							1
#define INCLUDE_xTaskResumeFromISR						0
#define INCLUDE_xTaskGetCurrentTaskHandle				1
#define INCLUDE_xTaskGetSchedulerState					0
#define INCLUDE_xSemaphoreGetMutexHolder				1
#define INCLUDE_xTimerPendFunctionCall					1

/* This demo makes use of one or more example stats formatting functions.  These
 * format the raw data provided by the uxTaskGetSystemState() function in to
 * human readable ASCII form.  See the notes in the implementation of vTaskList()
 * within FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS			1

/* Dimensions a buffer that can be used by the FreeRTOS+CLI command interpreter.
 * See the FreeRTOS+CLI documentation for more information:
 * http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/ */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE				2048

/* Interrupt priority configuration follows...................... */

/* Use the system definition, if there is one. */

#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS								__NVIC_PRIO_BITS
#else
#define configPRIO_BITS								3	 /* 8 priority levels. */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
 * function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			7

/* The highest interrupt priority that can be used by any interrupt service
 * routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT
 * CALL INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A
 * HIGHER PRIORITY THAN THIS! (higher priorities are lower numeric values). */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	2

/* Interrupt priorities used by the kernel port layer itself.  These are generic
 * to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY					( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY			( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )


/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names - or at least those used in the unmodified vector table. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* The #ifdef guards against the file being included from IAR assembly files. */
#if (__IASMARM__ != 1)

/* Constants related to the generation of run time stats. */
#define configGENERATE_RUN_TIME_STATS					1
#if configGENERATE_RUN_TIME_STATS
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() 		//( ulHighFrequencyTimerTicks = 0UL )
#define portGET_RUN_TIME_COUNTER_VALUE() 				    (*((volatile uint32_t*)(0x41018000 + 0x014)))  /* AmebaLite TIMER0_REG_BASE: 0x42009000, COUNT REGISTER offset: 0x014 */
#undef	configUSE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY						1
#define portCONFIGURE_STATS_PEROID_VALUE				1000 //unit Ticks
#else
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()				xTickCount
#endif

#define configTICK_RATE_HZ								( ( TickType_t ) 1000 )

/* Enable tickless power saving. */
#define configUSE_TICKLESS_IDLE                 		1

/* In wlan usage, this value is suggested to use value less than 80 milliseconds */
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   		2

/* It's magic trick that let us can use our own sleep function */
#ifndef CONFIG_MP_SHRINK
#define configPRE_SLEEP_PROCESSING( x )         		( pmu_pre_sleep_processing((uint32_t *)&x) )
#define configPOST_SLEEP_PROCESSING( x )        		( pmu_post_sleep_processing((uint32_t *)&x) )
#else
#define configPRE_SLEEP_PROCESSING( x )
#define configPOST_SLEEP_PROCESSING( x )
#endif

extern void pmu_pre_sleep_processing(uint32_t *expected_idle_time);
extern void pmu_post_sleep_processing(uint32_t *expected_idle_time);
extern int  pmu_ready_to_sleep(void);

/* It's magic trick that let us can enable/disable tickless dynamically */
#define traceLOW_POWER_IDLE_BEGIN()
#define traceLOW_POWER_IDLE_END()

/* system assert level controlled by rtk_assert.h header file. */
#ifndef __ASSEMBLER__
#include "rtk_assert.h"
//#define configASSERT(x)    rtk_assert(x)
#endif

#endif /* __IASMARM__ */
extern void TaskExitError(void);
#define configTASK_RETURN_ADDRESS TaskExitError
#endif /* FREERTOS_CONFIG_H */
