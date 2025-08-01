/** mbed Microcontroller Library
 ******************************************************************************
 * @file    timer_api.c
 * @author
 * @version V1.0.0
 * @date    2016-08-01
 * @brief   This file provides mbed API for gtimer.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "objects.h"
#include "timer_api.h"

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_TIMER
 *  @brief      MBED_TIMER driver modules.
 *  @{
 */

/** @defgroup MBED_TIMER_Exported_Functions MBED_TIMER Exported Functions
  * @{
  */

/* group: MBED_TIMER_Exported_Functions */
/* Static function in the group should not be extracted to the doxygen doc */
static u32 gtimer_timeout_handler(void *data)
{
	gtimer_t *obj = (gtimer_t *)data;
	uint32_t tid = obj->timer_id;
	gtimer_irq_handler handler;

	RTIM_INTClear(TIMx[tid]);

	if (!obj->is_periodcal) {
		gtimer_stop(obj);
	}

	if (obj->handler != NULL) {
		handler = (gtimer_irq_handler)obj->handler;
		handler(obj->hid);
	}

	return 0;
}

/**
  * @brief  Initialize the timer device, including timer registers and interrupt.
  * @param  obj: Timer object defined in application software.
  * @param  tid: General timer ID, which can be one of the following parameters:
  *     @arg TIMER0
  *     @arg TIMER1
  *     @arg TIMER2
  *     @arg TIMER3
  *     @arg TIMER4
  *     @arg TIMER5
  *     @arg TIMER6
  *     @arg TIMER7
  *     @arg TIMER8
  * @note AP TIMER0/1/2/3/4/5/6/7/8 are recommended.
  * @retval none
  */
void gtimer_init(gtimer_t *obj, uint32_t tid)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	assert_param(tid < GTIMER_MAX);
	RCC_PeriphClockCmd(APBPeriph_TIMx[tid], APBPeriph_TIMx_CLOCK[tid], ENABLE);

	obj->timer_id = tid;

	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Idx = (u8)tid;

	TIM_InitStruct.TIM_UpdateEvent = ENABLE; /* UEV enable */
	TIM_InitStruct.TIM_UpdateSource = TIM_UpdateSource_Overflow;
	TIM_InitStruct.TIM_ARRProtection = ENABLE;

	RTIM_TimeBaseInit(TIMx[tid], &TIM_InitStruct, TIMx_irq[tid], (IRQ_FUN) gtimer_timeout_handler, (u32)obj);
}

/**
  * @brief  Deinitialize the timer device, including interrupt and timer registers.
  * @param  obj: Timer object defined in application software.
  * @retval none
  */
void gtimer_deinit(gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param(tid < GTIMER_MAX);

	RTIM_DeInit(TIMx[tid]);
}

/**
  * @brief  Get counter value of the specified timer.
  * @param  obj: Timer object defined in application software.
  * @return Counter value.
  */
uint32_t gtimer_read_tick(gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param(tid < GTIMER_MAX);

	return (RTIM_GetCount(TIMx[tid]));
}

/**
  * @brief  Read current timer tick in microsecond.
  * @param  obj: Timer object defined in application software.
  * @return 64b tick time in microsecond(us).
  */
uint64_t gtimer_read_us(gtimer_t *obj)   //need to be test in IAR(64bit computing)
{
	assert_param(obj->timer_id < 4);

	uint64_t time_us = 0;
	uint8_t clk_source = 0;

	switch (obj->timer_id) {
	case 0:
		clk_source = RCC_PeriphClockSourceGet(LTIM0);
		break;
	case 1:
		clk_source = RCC_PeriphClockSourceGet(LTIM1);
		break;
	case 2:
		clk_source = RCC_PeriphClockSourceGet(LTIM2);
		break;
	case 3:
		clk_source = RCC_PeriphClockSourceGet(LTIM3);
		break;
	}

	if (!clk_source) {
		time_us = (uint64_t)(gtimer_read_tick(obj) * ((float)1000000 / 32768));
	} else {
		time_us = gtimer_read_tick(obj);
	}

	return (time_us);
}

/**
  * @brief  Change period of the specified timer.
  * @param  obj: Timer object defined in application software.
  * @param  duration_us: Period to be set in microseconds.
  * @retval none
  */
void gtimer_reload(gtimer_t *obj, uint32_t duration_us)
{
	assert_param(obj->timer_id < 4);

	uint32_t tid = obj->timer_id;

	RTIM_ChangePeriodImmediate_us(TIMx[tid], tid, duration_us);
}

/**
  * @brief  Start the specified timer and enable update interrupt.
  * @param  obj: Timer object defined in application software.
  * @retval none
  */
void gtimer_start(gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param(tid < GTIMER_MAX);

	RTIM_INTConfig(TIMx[tid], TIM_IT_Update, ENABLE);
	RTIM_Cmd(TIMx[tid], ENABLE);
}

/**
  * @brief Start the specified timer in one-shot mode with specified period and interrupt handler.
  * @param  obj: Timer object defined in application software.
  * @param  duration_us: Period to be set in microseconds.
  * @param  handler: User-defined IRQ callback function.
  * @param  hid: User-defined IRQ callback parameter.
  * @retval none
  * @note In one-shot mode, timer will stop counting the first time counter overflows.
  */
void gtimer_start_one_shout(gtimer_t *obj, uint32_t duration_us, void *handler, uint32_t hid)
{
	assert_param(obj->timer_id < GTIMER_MAX);

	obj->is_periodcal = FALSE;
	obj->handler = handler;
	obj->hid = hid;

	gtimer_reload(obj, duration_us);
	gtimer_start(obj);
}

/**
  * @brief Start the specified timer in periodical mode with specified period and interrupt handler.
  * @param  obj: Timer object defined in application software.
  * @param  duration_us: Period to be set in microseconds.
  * @param  handler: User-defined IRQ callback function.
  * @param  hid: User-defined IRQ callback parameter.
  * @retval none
  * @note In periodical mode, timer will restart from 0 each time the counter overflows.
  */
void gtimer_start_periodical(gtimer_t *obj, uint32_t duration_us, void *handler, uint32_t hid)
{
	assert_param(obj->timer_id < GTIMER_MAX);

	obj->is_periodcal = TRUE;
	obj->handler = handler;
	obj->hid = hid;

	gtimer_reload(obj, duration_us);
	gtimer_start(obj);
}

/**
  * @brief  Disable the specified timer peripheral.
  * @param  obj: Timer object defined in application software.
  * @retval none
  */
void gtimer_stop(gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param(tid < GTIMER_MAX);

	RTIM_Cmd(TIMx[tid], DISABLE);
}

/** @} */
/** @} */
/** @} */