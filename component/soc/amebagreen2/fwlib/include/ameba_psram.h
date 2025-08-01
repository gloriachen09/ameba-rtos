/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PSRAM_H_
#define _AMEBA_PSRAM_H_

#include "ameba_spic.h"
#include "ameba_psphy.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PSRAM
* @{
*/

/* AUTO_GEN_START */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */

/* Exported types ------------------------------------------------------------*/
/** @defgroup PSRAM_Exported_Types PSRAM Exported Types
  * @{
  */

/**
  * @brief  PSRAM PHY Init Structure Definition
  */
typedef struct {
	//0x04 REG_CAL_PAR
	u32 PSRAMP_PRE_CAL_PHASE;		/*!< one-hot phase select, 4-tap delay line to adjust rwds phase before calibration for large freq range or skew
											can be set to 0, bypassing the default 1-tap delay*/
	u32 PSRAMP_CFG_CAL_JMIN;			/*!< Specifies Minimum J . Hardware may adjust J , if J reach this value , an interrup will be raised to alerm software. */
	u32 PSRAMP_CFG_CAL_JMAX;			/*!< Specifies Maximum J . Hardware may adjust J , but will not exceed this value.*/
	u32 PSRAMP_CFG_CAL_J;			/*!< Specifies Initial value J calibrated by software */
	u32 PSRAMP_CFG_CAL_N;			/*!< Specifies Initial value N calibrated by software . */

	//0x18 REG_PHY_CTRL
	u32 PSRAMP_SW_RST_N;		/*!< low active, except for phy register(using system reset), psram software reset: psram, phy_ctrl, calibrationa */
	u32 PSRAMP_RWDS_CHK_OPT;		/*!<R/W 1'b0  0: synced psram_rwds_i direct to controller 1: only check psram_rwds_i at last valid cycle of CA stage, sync this pulse to controller */
	u32 PSRAMP_RWDS_PD_EN;		/*!<R/W 1'b0  psram_phy can control PD pin of rwds iopad, bypass high-Z state at CMD/ADDR stage */
	u32 PSRAMP_LATENCY_TYPE; 		/*!< set this bit according to spic & psram latency settings 0: variable latency 1: fixed latency */
	u32 PSRAMP_CAL_RWDS_PHASE; 		/*!< Specifies the byte size of an address in read/write command in auto_mode. */
	u32 PSRAMP_RFIFO_RDY_DLY;		/* variable latency: delay read data cycles(phy clk) after detecting first DQS rising edge, must < 4
										fixed latency: delay read data cycles(phy clk) after asserting spi_rx_data_en, recommend 4 ~ 6
										4*read_latency - rd_dummy_length + rfifo_rdy_dly = in_physical_cyc */

	u32 PSRAMP_DQ16_EN;
	u32 DDR_PAD_CTRL1;
	u32 DDR_PAD_CTRL2;
} PSPHY_InitTypeDef;
#if 0
/**
  * @brief  PSRAM Init Structure Definition
  */
typedef struct {
	// 0x0 control reg
	u32 PSRAMC_ddr_en;			/*!< Specifies DDR mode in CMD_CH/DATA_CH/ADDR_CH. */
	u32 PSRAMC_ch;				/*!< Specifies channel number of cmd/data/addr phase in transmitting or receiving data. */
	u32 PSRAMC_ck_mtims;			/*!< Specifies the check times */

	//0x14 clk div
	u32 PSRAMC_clk_div;			/*!< Specifies spi_sclk divider value. spi_sclk = The frequency of bus_clk / (2*SCKDV) */

	//0x118
	u32 PSRAMC_user_rd_dummy_len;			/*!< Specifies rd latency in user mode */
	u32 PSRAMC_user_cmd_len;			/*!< Specifies cmd len in user mode */
	u32 PSRAMC_user_addr_len;			/*!< Specifies addr len in user mode */

	//0x11C delay in auto mode and cmd length
	u32 PSRAMC_auto_rd_latency;		/*!< Specifies delay cycles for receiving data */
	u32 PSRAMC_auto_in_phy_cyc; 		/*!< Specifies the byte size of an address in read/write command in auto_mode. */
	u32 PSRAMC_auto_addr_len; 		/*!< Specifies the byte size of an address in read/write command in auto_mode. */
	u32 PSRAMC_PDEX_CMD0;
	u32 PSRAMC_PDEX_CMD1;
	u32 PSRAMC_PDE_CMD_LEN;
	u32 PSRAMC_PDX_CMD_LEN;
	u32 PSRAMC_PDX_CS_H_CNT;
	u32 PSRAMC_PDE_CS_H_CNT;
	//0x120 valid cmd
	u32 PSRAMC_frd_single;		/*!< Specifies whether fast read command is a valid command to execute . */
	u32 PSRAMC_seq_rd_en;		/*!< Specifies whether to enable read sequential transaction read function in auto mode. */
	u32 PSRAMC_seq_wr_en;		/*!< Specifies whether to enable write sequential transaction read function in auto mode. */

	//0x138 Timing para
	u32 PSRAMC_rd_dum_len; 		/*!< Specifies the dummy cycles between sending read cmd to spic. to avoid the timing violation of CS high time*/
	u32 PSRAMC_wr_dum_len;		/*!< Specifies the dummy cycles between sending write cmd to spic. to avoid the timing violation of CS high time.*/
	u32 PSRAMC_cs_active_hold;	/*!< Specifies the active hold time after SCLK rising edge, max 4.*/
	u32 PSRAMC_cs_seq_timeout;	/*!< Specifies the timeout setting of auto cmd after aeq read cmd.*/
	u32 PSRAMC_cs_tcem;			/*!< Set to chop auto cmd when CSN low pulse width = (tcem*32)*busclk.*/
	u32 PSRAMC_cs_css;			/*Chip Select Setup to next CK Rising Edge*/
	//0x134 device info
	u32 PSRAMC_page_size;		/*!< Specifies the page size of psram, device page size = 2^PSRAMC_page_size*/
	u32 PSRAMC_wr_page_en;		/*!< Specifies whether to chop the burst write cmd across page boundaries in auto mode */
	u32 PSRAMC_rd_page_en;		/*!< Specifies whether to chop the burst read cmd across page boundaries in auto mode*/
	u32 PSRAMC_atom_size;		/*!< Set (0) when the atomic size of SPI device is 1-byte.Set (1) when the atomic size of SPI device is 2-byte.
									(Usually using when OPI and DDR are enable) */
	u32 PSRAMC_qspi_psram;		/*!< choose device type: psram */
	u32 PSRAMC_jedec_p2cmf;		/*!< Set (1) when the SPI cmd is JEDEC Profile 2.0 Command Modifier Formats. (6-byte command and address)*/
	u32 PSRAMC_data_unit_2b;	/*!< Set (1) when the SPI device stores 2-byte data for each address.*/

	//0x110 hw status
	u32 PSRAMC_so_dnum; 			/*!< Specifies single channel(0) connection of multi channels(1) */
	u32 PSRAMC_tx_fifo_entry; 			/*!< <5 when fifo entry = 32 */
	u32 PSRAMC_rx_fifo_entry; 		/*!< Specifies quad data read cmd */
	u32 PSRAMC_full_wr; 		/*!< Specifies whether to enable full write in auto mode, set 0 to use spi_dm to do partial write */
	u32 PSRAMC_dis_dm_ca; 		/*!< Specifies whether to Set to disable spi_dm_oe_n (let the device to drive DM) when SPIC pushing CMD and ADDR for both user mode and auto mode. */

	//0x13c wr latency
	u32 PSRAMC_auto_wr_latency;		/*!< Specifies delay cycles for writing data */

	//0xe0 rd cmd
	u32 PSRAMC_frd_cmd;			/*!< Specifies fast read cmd in auto mode */

	//0xf4 wr cmd
	u32 PSRAMC_wr_cmd;			/*!< Specifies write cmd in auto mode */
	u32 RD_WEIGHT;				/*Set to determine the read and write priority. RD_WEIGHT should >= 2 (if set to 0 or 1, RD_WEIGHT will use the previous value).*/
	u32 WR_VL_EN;				/*Set to enable variable write latency (must enable RD_VL_EN at the same time). SPIC will set variable write latency according to RWDS.*/
	u32 RD_VL_EN;				/*Set to enable variable read latency. SPIC will not sample read data until rx_data_valid = 1.If RD_VL_EN = 0, SPIC will sample read data according to internal counter.
								Note: when RD_VL_EN = 1, 0x11C[15: 12] IN_PHYSICAL_CYC should be set to 0 and 0x138[31: 24] CS_TCEM should be set appropriately to avoid TCEM violation.*/
} PCTL_InitTypeDef;
#endif
/**
  * @brief  PSRAM Calibration Structure Definition
  */
typedef struct {
	u32 phase;
	int window_size;
	u32 CAL_N;
	u32 CAL_J;
	u32 CAL_JMAX;
} PCAL_InitTypeDef;

/**
  * @}
  */

/** @defgroup PSRAM_DQnum_define
 * @{
 */
enum PSRAM_DQ_Type {
	PSRAM_DEVICE_DQ8 = 0x1000,
	PSRAM_DEVICE_DQ16 = 0x2000,
};
/**
  * @}
  */

/** @defgroup PSRAM_Size_define
 * @{
 */
enum PSRAM_Size_Info {
	PSRAM_SIZE_32Mb = 0x100,
	PSRAM_SIZE_64Mb = 0x200,
	PSRAM_SIZE_128Mb = 0x300,
	PSRAM_SIZE_256Mb = 0x400,
	PSRAM_SIZE_512Mb = 0x500,

};
/**
  * @}
  */

/** @defgroup PSRAM_Clk_Limit_define
  * @{
  */
enum PSRAM_CLK_LIMIT {
	PSRAM_DEVICE_CLK_166 = 0x10,
	PSRAM_DEVICE_CLK_200 = 0x20,
	PSRAM_DEVICE_CLK_250 = 0x30,
};
/**
  * @}
  */
/** @defgroup PSRAM_Page_Size_define
  * @{
  */
enum PSRAM_PAGE_SIZE {
	PSRAM_PAGE128 = 0x7,
	PSRAM_PAGE512 = 0x9,
	PSRAM_PAGE1024 = 0xa,
	PSRAM_PAGE2048 = 0xb,
};
/**
  * @}
  */


/** @defgroup CHIPINFO_TypeDef
  * @{
  */
typedef struct {
	u32 Psram_CLK_Limit;
	u32 Psram_Latency;
	u32 psram_type;
} PSRAM_Latency;
/**
  * @}
  */


/** @defgroup PSRAMINFO_TypeDef
  * @{
  */
typedef struct {
	u32 Psram_Vendor;
	u32 Psram_Clk_Limit;
	u32 Psram_Size;
	u32 Psram_Clk_Set;
	u32 Psram_Page_size;
	u32 PSRAMC_Clk_Unit;
	u32 Psram_Latency_Set;
	u32 Psram_CSHI;			//cs high between cmd
	u32 Psram_TRWR;			//cmd + tcss + cs high + addr
	u32 Psram_Resume_Cnt;	//for autogating phy resume
	u32 Psram_Type;
	u32 Psram_DQ16;
	u32 Psram_PDEX_CMD0;
	u32 Psram_PDEX_CMD1;
	u32 Psram_PDX_TIME;
	u32 Psram_PDE_TIME;
} PSRAMINFO_TypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup PSRAM_Exported_Constants PSRAM Exported Constants
  * @{
  */

/** @defgroup PSRAM_APM_Device_Register
  * @{
  */
#define PSRAM_READ_LATENCY_CODE(x)		((u8)(((x) & 0x07) << 2))		//100:200M, LC=7; 101:250M, LC=8
#define PSRAM_LT_SELECT(x)				((u8)(((x) & 0x01) << 5))		//0:variable 1:fix
#define PSRAM_Drive_Strength(x)			((u8)(((x) & 0x03) << 0))		//driving strength: "00" 25 ohms "01" 50ohms "10" 100homs "11" 200ohms
#define PSRAM_ENABLE_SLOW_REFRESH		((u8)(0x01 << 3))				//enable slow refresh when temperature allows
#define PSRAM_Refresh_Setting(x)		((u8)(((x) & 0x03) <<3))			//for DQ16 Refresh Frequency setting MR4[4:3]  x0: Always 4x Refresh (default) 01:Enables 1x Refresh when temperature allows (temperature<= 70C) 11: Enable 0.5x Refresh when temperature allows (temperature<= 50C)
#define PSRAM_WRITE_LATENCY_CODE(x)		((u8)(((x) & 0x07) << 5))		//001: 200M LC=7; 101:250M LC=8
#define PSRAM_ENBALE_DQ16				((u8)0x01 << 6)					//IO X8/X16 Mode MR8
/**
  * @}
  */

/** @defgroup PSRAM_WB_Device_Register
  * @{
  */
#define PSRAM_WB_BURST_LENGTH(x)		((u8)(((x) & 0x03) << 0))	/*00: 128byte 01:64byte 10:16byte 11:32byte */
#define PSRAM_WB_HyBURSE_EN				((u8) 0x01 << 2)	/* 0:hybrid burst sequencing 1:legacy wrapped burst manner */
#define PSRAM_WB_FIX_LATENCY_EN(x)		((u8)((x) & 0x01) << 3)	/* 0:Variable Latency 1:Fixed 2 times Initial Latency */
#define PSRAM_WB_INIT_LATENCY(x)		((u8)(((x) & 0x0f) << 4))	/*0010b:7 Clock Latency @ 250/200MHz Max Frequency  */
#define PSRAM_BIT8_WB_BURST_LENGTH(x)	((u8)(((x) & 0x01) << 0))
#define PSRAM_WB_RESERVER				((u8)(0x07 << 1))	/*When writing this register, these bits should be set to 1 for future compatibility */
#define PSRAM_WB_DRIVE_STRENGTH(x)		((u8)(((x) & 0x07) << 4))	/*001b - 115 ohms */
#define PSRAM_WB_DPD_EN(x)				((u8)(((x) & 0x01) << 7))	/*1:Normal operation 0:Writing 0 to CR0[15] causes the device to enter Deep Power Down (DPD) */
/**
  * @}
  */

/** @defgroup APM_PSRAM_Init_Latency
  * @{
  */
#define APM_WR_INIT_LATENCY_3CLK		0
#define APM_WR_INIT_LATENCY_4CLK		0x4
#define APM_WR_INIT_LATENCY_5CLK		0x2
#define APM_WR_INIT_LATENCY_6CLK		0x6
#define APM_WR_INIT_LATENCY_7CLK		0x1
#define APM_WR_INIT_LATENCY_8CLK		0x5
/**
  * @}
  */

/** @defgroup WB_PSRAM_Init_Latency
  * @{
  */
#define WB_WR_INIT_LATENCY_3CLK		0xe
#define WB_WR_INIT_LATENCY_4CLK		0xf
#define WB_WR_INIT_LATENCY_5CLK		0x0
#define WB_WR_INIT_LATENCY_6CLK		0x1
#define WB_WR_INIT_LATENCY_7CLK		0x2
#define WB_WR_INIT_LATENCY_10CLK	0x5

/**
  * @}
  */

/** @defgroup APM_PSRAM_Dummy_Latency
  * @{
  */
#define APM_WR_DUMMY_LATENCY_100		0x3
#define APM_WR_DUMMY_LATENCY_150		0x6
#define APM_WR_DUMMY_LATENCY_200		0x8
#define APM_WR_DUMMY_LATENCY_250		0xE

#define APM_WR_DUMMY_LATENCY	APM_WR_DUMMY_LATENCY_200	//Modify macro definition to switch PSRAM model
/**
  * @}
  */


/** @defgroup WB_PSRAM_TRWR
  * @{
  */

#define Psram_WB_TRWR133		38//ns
#define Psram_WB_TRWR166		36//ns
/**
  * @}
  */

/** @defgroup PSRAM_Type
  * @{
  */

#define	PSRAM_TYPE_WB955	0x0
#define	PSRAM_TYPE_WB957	0x1
#define	PSRAM_TYPE_WB958	0x2

/**
  * @}
  */


/** @defgroup WB_Drive_Strength
  * @{
  */
#define WB_Drive_Strength_000		0x0 //wb958:34ohms, wb957:25ohms, wb955:50ohms
#define WB_Drive_Strength_001		0x1 //wb958:115ohms, wb957:50ohms, wb955:35ohms
#define WB_Drive_Strength_010		0x2	//wb958:67ohms, wb957:100ohms, wb955:100ohms
#define WB_Drive_Strength_011		0x3	//wb958:46ohms, wb957:200ohms, wb955:200ohms
#define WB_Drive_Strength_100		0x4	//wb958:34ohms, wb957:300ohms
#define WB_Drive_Strength_101		0x5	//wb958:27ohms
#define WB_Drive_Strength_110		0x6	//wb958:22ohms
#define WB_Drive_Strength_111		0x7	//wb958:19ohms
/**
  * @}
  */

/** @defgroup APM_Drive_Strength
  * @{
  */
#define APM_Drive_Strength_25ohms		0x0
#define APM_Drive_Strength_50ohms		0x1
#define APM_Drive_Strength_100ohms		0x2
#define APM_Drive_Strength_200ohms		0x3
/**
  * @}
  */

/** @defgroup APM_refresh
  * @{
  */
#define APM_FAST_REFRESH		0x0
#define APM_SLOW_REFRESH		0x3
/**
  * @}
  */

/** @defgroup WB_Deep_Power_Down
 * @{
 */
#define WB_DPD_Enter		0x0
#define WB_NORMAL_OPE		0x1
/**
  * @}
  */

/** @defgroup WB_BURST_LEN
  * @{
  */
#define WB_BURST_LEN_16B		0x2// wb957:[8,1,0]=110
#define WB_BURST_LEN_32B		0x3// wb957:[8,1,0]=111
#define WB_BURST_LEN_64B		0x1// wb957:[8,1,0]=101
#define WB_BURST_LEN_128B		0x0// wb957:[8,1,0]=100
#define WB_BURST_LEN_2048B		0x3// wb957:[8,1,0]=011

/**
  * @}
  */

/** @defgroup PSRAM_Init_Latency
  * @{
  */
#define PSRAM_INIT_FIX_RD_LATENCY_CLK_250		16
#define PSRAM_INIT_FIX_RD_LATENCY_CLK_200		14

#define PSRAM_INIT_VL_RD_LATENCY_CLK_250		8
#define PSRAM_INIT_VL_RD_LATENCY_CLK_200		7

#define PSRAM_INIT_WR_LATENCY_CLK_250			8
#define PSRAM_INIT_WR_LATENCY_CLK_200			7

#define PSRAM_INIT_FIX_RD_LATENCY_CLK PSRAM_INIT_FIX_RD_LATENCY_CLK_200	//Modify macro definition to switch PSRAM model
#define PSRAM_INIT_VL_RD_LATENCY_CLK PSRAM_INIT_VL_RD_LATENCY_CLK_200	//Modify macro definition to switch PSRAM model
#define PSRAM_INIT_WR_LATENCY_CLK PSRAM_INIT_WR_LATENCY_CLK_200	//Modify macro definition to switch PSRAM model
/**
  * @}
  */

/** @defgroup PSRAMP_Latency_Type
  * @{
  */
#define PSRSAM_FIX_LATENCY			0x0	//0 for variable, 1 for fix

/**
  * @}
  */

/** @defgroup PHY_Latency_Definitions
  * @{
  */
#define PSPHY_RFIFO_READY_DELAY_VL		0x0
#define PSPHY_RFIFO_READY_DELAY_FIX		0x8
/**
  * @}
  */

/** @defgroup WB_PSRAM_Latency_Definitions
  * @{
  */
#define PSRAM_WB_INIT_FIX_RD_LATENCY_CLK	12
#define PSRAM_WB_INIT_VL_RD_LATENCY_CLK		6

#define PSRAM_WB_INIT_FIX_WR_LATENCY_CLK	12
#define PSRAM_WB_INIT_VL_WR_LATENCY_CLK		6


#define PSRAM_WB_INIT_FIX_RD_LATENCY_200CLK		14
#define PSRAM_WB_INIT_VL_RD_LATENCY_200CLK		7

#define PSRAM_WB_INIT_FIX_WR_LATENCY_200CLK		14
#define PSRAM_WB_INIT_VL_WR_LATENCY_200CLK		7

/**
  * @}
  */

/** @defgroup WB_PSRAM_TCEM
  * @{
  */
#define Psram_Tcem_T25	4000	//unit ns, (tcem*32)*busclk.
#define Psram_Tcem_T85	1000	//unit us, (tcem*32)*busclk.
#define Psram_Tcem_T25_APM	8000	//unit us, (tcem*32)*busclk.

/**
  * @}
  */


static inline int integer_ceil(int num, int den)
{
	return (num + den - 1) / den;
}

/** @defgroup PSRAM_PDEX_CMD
  * @{
  */
#define PSRAM_WB955_PDEX_CMD0	0x00010060
#define PSRAM_WB_PDEX_CMD0		0x0001FFE1
#define PSRAM_WB_PDEX_CMD1		0x60000100
#define PSRAM_APM_PDEX_CMD0		0x0006F0F0
#define PSRAM_APM_PDEX_CMD1		0xC0C00000
#define PSRAM_PDE_CMD_LEN		  0x8			//unit us
#define PSRAM_PDX_CMD_LEN(x)	integer_ceil(x * 60 , 1000)		//(SPIC_CLK_FRQ * tXPDPD)/1000

/**
  * @}
  */

#define CAL_PDEX_CS_H_CNT(spic_clk_frq, tres2) ({ \
    int pdx_cycle_num = (int)(spic_clk_frq) * (int)(tres2);         \
    int cr_pdx_cs_h_cnt = 0;                              \
    if (pdx_cycle_num > 0 && pdx_cycle_num <= 768) {      \
        cr_pdx_cs_h_cnt = integer_ceil(pdx_cycle_num, 256); \
    } else if (pdx_cycle_num > 768 && pdx_cycle_num <= 15872) { \
        cr_pdx_cs_h_cnt = integer_ceil(pdx_cycle_num, 512);     \
    } else if (pdx_cycle_num > 15872 && pdx_cycle_num <= 524288) { \
        cr_pdx_cs_h_cnt = integer_ceil(pdx_cycle_num, 16384) + 31; \
    }                                                     \
    cr_pdx_cs_h_cnt;                                      \
})


/* Exported functions --------------------------------------------------------*/
/** @defgroup PSRAM_Exported_Functions PSRAM Exported Functions
  * @{
  */
_LONG_CALL_	void PSRAM_CLK_Update(void);
_LONG_CALL_ void PSRAM_PHY_StructInit(PSPHY_InitTypeDef *PSPHY_InitStruct);
_LONG_CALL_ void PSRAM_PHY_Init(PSPHY_InitTypeDef *PSPHY_InitStruct);
_LONG_CALL_ void PSRAM_CTRL_Init(void);
_LONG_CALL_ void PSRAM_REG_Read(u32 type, u32 addr, u32 read_len, u8 *read_data, u32 CR, u32 DQ_16);
_LONG_CALL_ void PSRAM_REG_Write(u32 type, u32 addr, u32 write_len, u8 *write_data, u32 DQ_16);
_LONG_CALL_ u32 PSRAM_calibration(void);
_LONG_CALL_ void PSRAM_APM_DEVIC_Init(void);
_LONG_CALL_ void PSRAM_WB_DEVIC_Init(void);
_LONG_CALL_ void PSRAM_HalfSleep_PDEX(u32 NewState);
_LONG_CALL_ void set_psram_sleep_mode(void);
_LONG_CALL_ void set_psram_wakeup_mode(void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Other definitions --------------------------------------------------------*/

#define PSRAM_VENDOR_GET(x)			(x & 0xF0000)
#define PSRAM_DQNUM_GET(x)			(x & 0xF000)
#define PSRAM_SIZE_GET(x)			  (x & 0xF00)
#define PSRAM_CLK_LIMIT_GET(x)	(x & 0xF0)
#define PSRAM_PAGE_SIZE_GET(x)	(x & 0xF)


#define PSRAM_WB_IR0		0x0
#define PSRAM_WB_IR1		0x1
#define PSRAM_WB_CR0		0x0
#define PSRAM_WB_CR1		0x1

#define WB_RD_CMD_LINEAR		0xa0
#define WB_WR_CMD_LINEAR		0x20
#define WB_RD_CMD_WRAPPED		0x80
#define WB_WR_CMD_WRAPPED		0x00

#define APM_RD_CMD				0x2020
#define APM_WR_CMD				0xa0a0

#define Psram_RESUME_TIME		32		//unit ns, worst 50ns
#define Psram_IDLETIME_PLL		1		//unit us
#define Psram_IDLETIME_XTAL		10		//unit us
#define Psram_RESUMECNT_XTAL	3		//unit T, 3T * clk

#define Psram_WB_CSHI133		8
#define Psram_WB_CSHI166		6
#define Psram_APM_CSHI250		28
#define Psram_APM_CSHI200		24		//APM64 20ns
#define Psram_APM_CSHI166		18
#define Psram_APM_CSHI133		15

#define Psram_WB_TRWR133		38
#define Psram_WB_TRWR166		36
#define Psram_WB_TRWR200		35
#define Psram_WB_TRWR250		35

#define Psram_WB955_TEXTHS		70		//unit us
#define Psram_WB957_TEXTHS		100
#define Psram_WB958_TEXTHS		100
#define Psram_APM_TEXTHS		150

#define Psram_WB955_THSIN		0		//unit us
#define Psram_WB957_THSIN		3
#define Psram_WB958_THSIN		3
#define Psram_APM_THSIN		150


#define Psram_Tcem_T25	4000	//unit ns, (tcem*32)*busclk.
#define Psram_Tcem_T85	1000	//unit us, (tcem*32)*busclk.

#define PSRAM_PHY_PAD_DRIVING	0x18
#define PSRAM_PHY_VOL_TUNE		0x2C

#define PSRAM_BURST_BIT8_SET		0x1//for wb955 958,this bit is reserved. for wb957, CR0[8,1,0]: 111 32bytes, 011:2048bytes, 100: 128bytes
#define PSRAM_BURST_BIT8_CLR		0x0

#define PSRAM_CAL_DQ0_7				0x0
#define PSRAM_CAL_DQ8_15			0x1
#define PSRAM_CAL_DQ16_23			0x2
#define PSRAM_CAL_DQ24_31			0x3

#define PSRAM_DQ16_SL0			0x0
#define PSRAM_DQ16_SL1			0x1
#define PSRAM_DQ8_SL0			0x2


#define PSRAM_TYPE_APM		0
#define PSRAM_TYPE_WB		1

#define PSRAM_MASK_CAL_ITT 		PSPHY_MASK_CFG_CAL_INTR_MASK|\
								PSPHY_BIT_CFG_CAL_EN

#define PSRAM_CHECK_INT(x) (PSPHY_GET_CFG_CAL_NBDR_OVF(x))||\
							(PSPHY_GET_CFG_CAL_NBDR_ALMOST(x))||\
							(PSPHY_GET_CFG_CAL_ERR_UNSURE(x))||\
							(PSPHY_GET_CFG_CAL_ERR_ALREADY(x))||\
							(PSPHY_GET_CFG_CAL_ERR_ALMOST(x))



#define WB_RD_CMD				0xa0
#define WB_WR_CMD				0x20

#define APM_RD_CMD				0x2020
#define APM_WR_CMD				0xa0a0

#define Psram_Seq_timeout		0x2	//unit us, (CS_SEQ_TIMEOUT *4) busclk

#define Psram_WB_CSH			0x0	//spec 0 ns
#define Psram_APM_CSH			0x1	//spec min 2ns, spic max 16T

#define Psram_ClkDiv2			1

#define Psram_phy_Jmin			2

#define PSRAM_CHECK_WITH_TIMEOUT(condition, timeout) do {            \
    int _timeout = (timeout);                                  \
    while (_timeout--) {                                       \
        if (condition) {                                       \
            break;                                             \
        }                                                      \
        if (_timeout == 0) {                                   \
            RTK_LOGI(TAG, "Timeout reached at function: %s, line: %d, with condition: %d\n", \
                   __FUNCTION__, __LINE__, (condition));       \
        }                                                      \
    }                                                          \
} while (0)

extern u8 APM_WR_INIT_LATENCY_SPEC[6];

#define PSRAM_APM64_DQ8 	PSRAM_DEVICE_DQ8|PSRAM_DEVICE_CLK_200 | MEM_PSRAM_APM | PSRAM_SIZE_64Mb | PSRAM_PAGE1024
#define PSRAM_APM128_DQ16 	PSRAM_DEVICE_DQ16|PSRAM_DEVICE_CLK_250 | MEM_PSRAM_APM | PSRAM_SIZE_128Mb | PSRAM_PAGE1024
#define PSRAM_APM256_DQ16 	PSRAM_DEVICE_DQ16|PSRAM_DEVICE_CLK_250 | MEM_PSRAM_APM | PSRAM_SIZE_256Mb | PSRAM_PAGE2048

#define PSRAM_WB955_DQ8  	PSRAM_DEVICE_DQ8|PSRAM_DEVICE_CLK_200 | MEM_PSRAM_WB | PSRAM_SIZE_32Mb | PSRAM_PAGE128
#define PSRAM_WB957_DQ16 	PSRAM_DEVICE_DQ16|PSRAM_DEVICE_CLK_250 | MEM_PSRAM_WB | PSRAM_SIZE_128Mb | PSRAM_PAGE2048
#define PSRAM_WB958_DQ16 	PSRAM_DEVICE_DQ16|PSRAM_DEVICE_CLK_250 | MEM_PSRAM_WB | PSRAM_SIZE_256Mb | PSRAM_PAGE1024

#define ChipInfo_MemoryInfo() ChipInfo_MCM_Info()
/* MANUAL_GEN_END */
#endif
