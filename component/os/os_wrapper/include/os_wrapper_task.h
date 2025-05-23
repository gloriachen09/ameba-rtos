/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_TASK_H__
#define __OS_WRAPPER_TASK_H__

#define RTOS_TASK_MAX_PRIORITIES         (11)
#define RTOS_MINIMAL_STACK_SIZE          (1024)
#define RTOS_MINIMAL_SECURE_STACK_SIZE   (1024)

#define RTOS_SCHED_SUSPENDED             0x0UL
#define RTOS_SCHED_NOT_STARTED           0x1UL
#define RTOS_SCHED_RUNNING               0x2UL

#define RTOS_LOCAL_STORAGE_LWIP_INDEX    0

/**
 * @brief  task handle and function type
 */
typedef void *rtos_task_t;
typedef void (*rtos_task_function_t)(void *);
typedef struct {
	uint32_t over_flow_count;
	uint32_t time_on_entering;
} rtos_time_out_t;

/**
 * @brief  Start os kernel scheduler
 * @retval return RTK_SUCCESS Only
 */
int rtos_sched_start(void);

/**
 * @brief  Stop os kernel scheduler
 * @retval return RTK_SUCCESS Only
 */
int rtos_sched_stop(void);

/**
 * @brief  Suspend os kernel scheduler
 * @retval return RTK_SUCCESS Only
 */
int rtos_sched_suspend(void);

/**
 * @brief  Resume os kernel scheduler
 * @retval return RTK_SUCCESS Only
 */
int rtos_sched_resume(void);

/**
 * @brief  Get scheduler status.
 * @retval RTOS_SCHED_SUSPENDED / RTOS_SCHED_NOT_STARTED / RTOS_SCHED_RUNNING
 */
int rtos_sched_get_state(void);

/**
 * @brief  Create os level task routine.
 * @note   Usage example:
 * Create:
 *          rtos_task_t handle;
 *          rtos_task_create(&handle, "test_task", task, NULL, 1024, 2);
 * Suspend:
 *          rtos_task_suspend(handle);
 * Resume:
 *          rtos_task_resume(handle);
 * Delete:
 *          rtos_task_delete(handle);
 * @param  pp_handle: The handle itself is a pointer, and the pp_handle means a pointer to the handle.
 * @param  p_name: A descriptive name for the task.
 * @param  p_routine: Pointer to the task entry function. Tasks must be implemented to never return (i.e. continuous loop).
 * @param  p_param: Pointer that will be used as the parameter
 * @param  stack_size_in_byte: The size of the task stack specified
 * @param  priority: The priority at which the task should run (higher value, higher priority)
 * @retval RTK_SUCCESS(0) / RTK_FAIL(-1)
 */
int rtos_task_create(rtos_task_t *pp_handle, const char *p_name, rtos_task_function_t p_routine,
					 void *p_param, size_t stack_size_in_byte, uint16_t priority);

/**
 * @brief  Delete os level task routine.
 * @param  p_handle: Task handle. If a null pointer is passed, the task itself is deleted.
 * @retval return RTK_SUCCESS Only
 */
int rtos_task_delete(rtos_task_t p_handle);

/**
 * @brief  Suspend os level task routine.
 * @param  p_handle: Task handle.
 * @retval return RTK_SUCCESS Only
 */
int rtos_task_suspend(rtos_task_t p_handle);

/**
 * @brief  Resume os level task routine.
 * @param  p_handle: Task handle.
 * @retval return RTK_SUCCESS Only
 */
int rtos_task_resume(rtos_task_t p_handle);

/**
 * @brief  Yield current os level task routine.
 * @retval return RTK_SUCCESS Only
 */
int rtos_task_yield(void);

/**
 * @brief  Get current os level task routine handle.
 * @retval The task handle pointer
 */
rtos_task_t rtos_task_handle_get(void);

/**
 * @brief  Get os level task routine priority.
 * @param  p_handle: Task handle.
 * @retval Task priority value
 */
uint32_t rtos_task_priority_get(rtos_task_t p_handle);

/**
 * @brief  Set os level task routine priority.
 * @param  p_handle: Task handle.
 * @param  priority: The priority at which the task should run (higher value, higher priority)
 * @retval return RTK_SUCCESS Only
 */
int rtos_task_priority_set(rtos_task_t p_handle, uint16_t priority);

/**
 * @brief  Print current task status
 */
void rtos_task_out_current_status(void);

/**
 * @brief  Allocate a secure context for the task.
 * @note   trustzone is required. Otherwise it's just an empty implementation.
 */
void rtos_create_secure_context(uint32_t size);

/**
 * @brief  The kernel does not use the pointers itself, so the application writer can use the pointers for any purpose they wish.
 *         The following function is used to set a pointer.
 * @param  p_handle: Task handle.
 * @param  index: the index of the LocalStorage array in task TCB
 * @param  p_param: Pointer to memory the caller give
 */
void rtos_task_set_thread_local_storage_pointer(rtos_task_t p_handle,  uint16_t index, void *p_param);

/**
 * @brief  The kernel does not use the pointers itself, so the application writer can use the pointers for any purpose they wish.
 *         The following function is used to query a pointer.
 * @param  p_handle: Task handle.
 * @param  index: the index of the LocalStorage array in task TCB
 * @retval Pointer to memory the caller previously give
 */
void *rtos_task_get_thread_local_storage_pointer(rtos_task_t p_handle,  uint16_t index);

/**
 * @brief  wrapper function for FreeRTOS API: vTaskSetTimeOutState
 * If a task enters and exits the Blocked state more than once while it is waiting for the event to occur then the timeout used
 * each time the task enters the Blocked state must be adjusted to ensure the total of all the time spent in the Blocked state
 * does not exceed the originally specified timeout period. rtos_task_check_for_time_out() performs the adjustment, taking into
 * account occasional occurrences such as tick count overflows, which would otherwise make a manual adjustment prone to error.
 * rtos_task_set_time_out_state() is used with rtos_task_check_for_time_out(). rtos_task_set_time_out_state() is called to set
 * the initial condition, after which rtos_task_check_for_time_out() can be called to check for a timeout condition,
 * and adjust the remaining block time if a timeout has not occurred.
 * @param  p_rtos_time_out: A pointer to a structure that will be initialized to hold information necessary to determine if a timeout has occurred.
 */
void rtos_task_set_time_out_state(rtos_time_out_t *const p_rtos_time_out);

/**
 * @brief  wrapper function for FreeRTOS API: xTaskCheckForTimeOut
 * @param  p_rtos_time_out: A pointer to a structure that holds information necessary to determine if a timeout has occurred.
 * 							p_rtos_time_out is initialized using rtos_task_set_time_out_state().
 * @param  p_ms_to_wait: Used to pass out an adjusted block time in ms, which is the block time that remains after taking into account
 * 						 the time already spent in the Blocked state.
 * @retval TRUE: a timeout has occurred, and no block time remains
 * @retval FALSE: not timeout. some block time remains (pass out by p_ms_to_wait), so a timeout has not occurred.
 */
int rtos_task_check_for_time_out(rtos_time_out_t *const p_rtos_time_out, uint32_t *p_ms_to_wait);

#endif
