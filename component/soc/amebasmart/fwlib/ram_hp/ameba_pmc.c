/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "PMC";
_OPTIMIZE_NONE_
void SOCPS_SleepPG(void)
{
	u32 nDeviceIdOffset = 0;

	//InterruptDis(UART_LOG_IRQ);
	Img2EntryFun0.RamWakeupFun = SOCPS_WakeFromPG_HP;

	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);

		RTK_LOGD(TAG, "DBG: KM4 Sleep PG blocked because Dev %lx  busy\n", nDeviceIdOffset);

		return;
	}

	SOCPS_SleepPG_LIB();

	/* indicate KM4 status immediately after mem is ready, or km0 may miss 32k cmd when km4 resume wifi */
	HAL_WRITE8(SYSTEM_CTRL_BASE_LP, REG_LSYS_NP_STATUS_SW,
			   HAL_READ8(SYSTEM_CTRL_BASE_LP, REG_LSYS_NP_STATUS_SW) | LSYS_BIT_NP_RUNNING);

	pmu_exec_wakeup_hook_funs(PMU_MAX);


}

void SOCPS_SleepCG(void)
{

	u32 nDeviceIdOffset = 0;

	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);

		RTK_LOGD(TAG, "DBG: KM4 Sleep CG blocked because Dev %lx  busy\n", nDeviceIdOffset);

		return;
	}
	SOCPS_SleepCG_LIB();

	HAL_WRITE8(SYSTEM_CTRL_BASE_LP, REG_LSYS_NP_STATUS_SW,
			   HAL_READ8(SYSTEM_CTRL_BASE_LP, REG_LSYS_NP_STATUS_SW) | LSYS_BIT_NP_RUNNING);

	/* exec sleep hook functions */
	pmu_exec_wakeup_hook_funs(PMU_MAX);
}
/**
  *  @brief Get dslp wake status.
	* @param none:

	* @retval DslpWake status.
	* 		   @arg TRUE: boot from deep sleep
	* 		   @arg FALSE: boot from power on
  */
u32 SOCPS_DsleepWakeStatusGet(void)
{
	u32 Temp = BOOT_Reason();

	if (Temp & AON_BIT_RSTF_DSLP) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
  * @brief  LP wake NP by IPC
  * @param  None
  * @note  ipc_msg_temp.msg represents who wakes up KM4
  		* ipc_msg_temp.msg = 0: FW wakeup KM4
  * @retval None
  */
void SOCPS_LPWHP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_LP_TO_NP, IPC_L2N_WAKE_NP);

	u32 type = ipc_msg_temp->msg;

	if (type == FW_LPWNP_IPC) {
		//RTK_LOGD(TAG, "%s: FW wakeup KM4 via IPC \n", __func__);
	}

}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_LPWHP_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
	.Rxfunc = SOCPS_LPWHP_ipc_int,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_LP_TO_NP,
	.IPC_Channel = IPC_L2N_WAKE_NP
};

