/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/**
  * @brief  set AON timer for wakeup.
  * @param  SDuration: sleep time, unit is ms
  * @note wakeup state:sleep PG & CG & deep sleep
  * @note This is 100KHz timer, max counter = 0xFFFFFFFF/100000/60 = 715min
  * @retval None
  */
void AONTimer_Setting(u32 SDuration)
{
	if (SDuration == 0) {
		return;
	}

	SDuration = SDuration * 100;

	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_TIM_SET, AON_TIM_CNT_CUR(SDuration));
}

/**
  * @brief  get current AON timer.
  * @param  None
  * @note wakeup state:sleep PG & CG & deep sleep
  * @retval The current Aon Timer counter, the unit is ms.
  */
u32 AONTimer_Get(void)
{
	u32 Rtemp = 0;

	/* use 32K */
	Rtemp = AON_GET_TIM_CNT_CUR(HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_TIM_CNT));
	Rtemp = Rtemp / 100;
	return Rtemp;
}


/**
  * @brief  enable AON timer interrupt.
  * @param  Status: interrupt status
  * @retval None
  */
void AONTimer_INT(u32 Status)
{
	u32 Rtemp = 0;

	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_TIM_IER);
	if (ENABLE == Status) {
		Rtemp |= AON_BIT_TIM_IE;
	} else {
		Rtemp &= (~AON_BIT_TIM_IE);

	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_TIM_IER, Rtemp);
}

/**
  * @brief  Clear aontimer interrupt.
  * @retval   None
  */
void AONTimer_ClearINT(void)
{
	u32 temp = 0;

	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_TIM_ISR);
	temp |= AON_BIT_TIM_ISR;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_TIM_ISR, temp);
}
