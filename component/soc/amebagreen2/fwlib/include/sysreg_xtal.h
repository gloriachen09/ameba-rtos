/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_XTAL_H_
#define _SYSREG_XTAL_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup XTAL
  * @brief XTAL driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup XTAL_Register_Definitions XTAL Register Definitions
  * @{
  */

/** @defgroup XTAL_ANAPAR_POW_XTAL_0
  * @brief
  * @{
  */
#define XTAL_MASK_BG2HPMOD_CYC   ((u32)0x000000FF << 16)           /*!< R/W 0x0111  XTAL arb bandgap enable to hp mode delay cycle x4M*/
#define XTAL_BG2HPMOD_CYC(x)     (((u32)((x) & 0x000000FF) << 16))
#define XTAL_GET_BG2HPMOD_CYC(x) ((u32)(((x >> 16) & 0x000000FF)))
#define XTAL_BIT_XMD_DONE        ((u32)0x00000001 << 9)            /*!< R 0x0  xmd change ok status*/
#define XTAL_BIT_CKRDY           ((u32)0x00000001 << 8)            /*!< R 0x0  xtal clock ready status*/
#define XTAL_MASK_STATE          ((u32)0x0000000F << 4)            /*!< R 0x0  0 to 1 Enable xtal*/
#define XTAL_STATE(x)            (((u32)((x) & 0x0000000F) << 4))
#define XTAL_GET_STATE(x)        ((u32)(((x >> 4) & 0x0000000F)))
#define XTAL_BIT_POW_XTAL        ((u32)0x00000001 << 1)            /*!< R 0x0  0 to 1 Enable xtal*/
#define XTAL_BIT_POW_BG          ((u32)0x00000001 << 0)            /*!< R 0x0  Enable BG  (BG for xtal and AFE)  (有option關)*/
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_ON_0
  * @brief
  * @{
  */
#define XTAL_MASK_SC_INIT   ((u32)0x0000007F << 24)           /*!< R/W 0x0000000  0: reduce cap, 1: increase cap. (220fF per unit) [efuse for BT only ] 有外掛電容設全0*/
#define XTAL_SC_INIT(x)     (((u32)((x) & 0x0000007F) << 24))
#define XTAL_GET_SC_INIT(x) ((u32)(((x >> 24) & 0x0000007F)))
#define XTAL_MASK_SC_XO     ((u32)0x0000007F << 17)           /*!< R/W 0x1000000  1: reduce cap, 0: increase cap. (220fF per unit) [efuse for BT only ]*/
#define XTAL_SC_XO(x)       (((u32)((x) & 0x0000007F) << 17))
#define XTAL_GET_SC_XO(x)   ((u32)(((x >> 17) & 0x0000007F)))
#define XTAL_MASK_SC_XI     ((u32)0x0000007F << 10)           /*!< R/W 0x1000000  1: reduce cap, 0: increase cap. (220fF per unit) [efuse for BT only ]*/
#define XTAL_SC_XI(x)       (((u32)((x) & 0x0000007F) << 10))
#define XTAL_GET_SC_XI(x)   ((u32)(((x >> 10) & 0x0000007F)))
#define XTAL_MASK_GMN       ((u32)0x0000001F << 5)            /*!< R/W 0x11111  GMN: 0~31*/
#define XTAL_GMN(x)         (((u32)((x) & 0x0000001F) << 5))
#define XTAL_GET_GMN(x)     ((u32)(((x >> 5) & 0x0000001F)))
#define XTAL_MASK_GMP       ((u32)0x0000001F << 0)            /*!< R/W 0x11111  GMP: 0~31*/
#define XTAL_GMP(x)         (((u32)((x) & 0x0000001F) << 0))
#define XTAL_GET_GMP(x)     ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_ON_1
  * @brief
  * @{
  */
#define XTAL_MASK_DUMMY2          ((u32)0x00000003 << 30)           /*!< R/W 0x11  Reserved for future use.*/
#define XTAL_DUMMY2(x)            (((u32)((x) & 0x00000003) << 30))
#define XTAL_GET_DUMMY2(x)        ((u32)(((x >> 30) & 0x00000003)))
#define XTAL_MASK_AAC_OPCUR       ((u32)0x00000003 << 28)           /*!< R/W 0x11  00: 200nA , 200n/step   For AAC robust 11*/
#define XTAL_AAC_OPCUR(x)         (((u32)((x) & 0x00000003) << 28))
#define XTAL_GET_AAC_OPCUR(x)     ((u32)(((x >> 28) & 0x00000003)))
#define XTAL_MASK_VREF_SEL        ((u32)0x0000001F << 23)           /*!< R/W 0x11000  Analog AAC VREF, 390mV~700mV, 10mV/step 11000: Vpp 500mV*/
#define XTAL_VREF_SEL(x)          (((u32)((x) & 0x0000001F) << 23))
#define XTAL_GET_VREF_SEL(x)      ((u32)(((x >> 23) & 0x0000001F)))
#define XTAL_BIT_LPS_DIVISOR      ((u32)0x00000001 << 22)           /*!< R/W 0x0  XTAL LPS clock divider  0: XTAL/128(40M), 1: XTAL/64*/
#define XTAL_BIT_CKDIGI_SEL       ((u32)0x00000001 << 21)           /*!< R/W 0x1  1: DIGI CLK from self-bias 0: DIGI CLK from Schmitt*/
#define XTAL_BIT_EN_XTAL_LPS_CLK  ((u32)0x00000001 << 20)           /*!< R/W 0x1  1: enable XTAL LPS clock  與anapar_xtal_on[43]都是312.5KHz enable*/
#define XTAL_BIT_EN_XTAL_SCHMITT  ((u32)0x00000001 << 19)           /*!< R/W 0x1  1: enable XTAL SCHMITT*/
#define XTAL_MASK_LDO_VREF        ((u32)0x00000007 << 16)           /*!< R/W 0x011  (起震前) 111: 1.1V  110: 1.05V  101:1V  100:0.95V 011: 0.9V  010: 0.85V   001: 0.8V  000: 0.75V*/
#define XTAL_LDO_VREF(x)          (((u32)((x) & 0x00000007) << 16))
#define XTAL_GET_LDO_VREF(x)      ((u32)(((x >> 16) & 0x00000007)))
#define XTAL_MASK_SEL_TOK         ((u32)0x00000007 << 13)           /*!< R/W 0x100  xtal起振过程 reg value: cycles / 40MHz / 26MHz 000: 2^10 / 0.025m / 0.05 001: 2^11 / 0.05m / 0.1m 010: 2^12 / 0.1m / 0.2m 011: 2^13 / 0.2m / 0.3m 100: 2^14 / 0.4m / 0.6m 101: 2^15 / 0.8m / 1.2m 110: 2^16 / 1.6m / 2.5m 111: 2^17 / 3.2m / 4.9m xtal mode switch过程 reg value: cycles / 40MHz / 26MHz 000: 2^10 / 0.0125m / 0.025 001: 2^11 / 0.025m / 0.05m 010: 2^12 / 0.05m / 0.1m 011: 2^13 / 0.1m / 0.2m 100: 2^14 / 0.2m / 0.3m 101: 2^15 / 0.4m / 0.6m 110: 2^16 / 0.8m / 1.25m 111: 2^17 / 1.6m / 2.45m*/
#define XTAL_SEL_TOK(x)           (((u32)((x) & 0x00000007) << 13))
#define XTAL_GET_SEL_TOK(x)       ((u32)(((x >> 13) & 0x00000007)))
#define XTAL_BIT_EN_XTAL_DRV_LPS  ((u32)0x00000001 << 12)           /*!< R/W 0x1  LPS enable clock source control, 1:enable, 0: gated 與anapar_xtal_on[51]都是312.5KHz enable*/
#define XTAL_BIT_EN_XTAL_DRV_DIGI ((u32)0x00000001 << 11)           /*!< R/W 0x1  DIGI enable clock source control, 1:enable, 0: gated*/
#define XTAL_BIT_EN_XTAL_DRV_USB  ((u32)0x00000001 << 10)           /*!< R/W 0x1  USB enable clock source control, 1:enable, 0: gated*/
#define XTAL_BIT_EN_XTAL_DRV_AFE  ((u32)0x00000001 << 9)            /*!< R/W 0x0  AFE enable clock source control, 1:enable, 0: gated (for WL AFE PLL)*/
#define XTAL_BIT_EN_XTAL_DRV_RF2  ((u32)0x00000001 << 8)            /*!< R/W 0x0  RF2 enable clock source control, 1:enable, 0: gated (for BT)*/
#define XTAL_BIT_EN_XTAL_DRV_RF1  ((u32)0x00000001 << 7)            /*!< R/W 0x1  RF1 enable clock source control, 1:enable, 0: gated (for WL SYN)*/
#define XTAL_MASK_SC_LPS          ((u32)0x0000007F << 0)            /*!< R/W 0x0000000  0: reduce cap, 1: increase cap. (160fF per unit) [efuse for BT only ] 若有頻偏再修改*/
#define XTAL_SC_LPS(x)            (((u32)((x) & 0x0000007F) << 0))
#define XTAL_GET_SC_LPS(x)        ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_ON_2
  * @brief
  * @{
  */
#define XTAL_MASK_SEL_TOK_FST          ((u32)0x00000007 << 29)           /*!< R/W 0x0  used for HP2NM or NM2HP xtal起振过程 reg value: cycles / 40MHz / 26MHz 000: 2^10 / 0.025m / 0.05 001: 2^11 / 0.05m / 0.1m 010: 2^12 / 0.1m / 0.2m 011: 2^13 / 0.2m / 0.3m 100: 2^14 / 0.4m / 0.6m 101: 2^15 / 0.8m / 1.2m 110: 2^16 / 1.6m / 2.5m 111: 2^17 / 3.2m / 4.9m xtal mode switch过程 reg value: cycles / 40MHz / 26MHz 000: 2^10 / 0.0125m / 0.025 001: 2^11 / 0.025m / 0.05m 010: 2^12 / 0.05m / 0.1m 011: 2^13 / 0.1m / 0.2m 100: 2^14 / 0.2m / 0.3m 101: 2^15 / 0.4m / 0.6m 110: 2^16 / 0.8m / 1.25m 111: 2^17 / 1.6m / 2.45m*/
#define XTAL_SEL_TOK_FST(x)            (((u32)((x) & 0x00000007) << 29))
#define XTAL_GET_SEL_TOK_FST(x)        ((u32)(((x >> 29) & 0x00000007)))
#define XTAL_MASK_DUMMY                ((u32)0x00000003 << 12)           /*!< R/W 0x0  Dummy control bits*/
#define XTAL_DUMMY(x)                  (((u32)((x) & 0x00000003) << 12))
#define XTAL_GET_DUMMY(x)              ((u32)(((x >> 12) & 0x00000003)))
#define XTAL_BIT_EN_XTAL_BDRF          ((u32)0x00000001 << 11)           /*!< R/W 0x1  Balance dynamic RF for duty cycle*/
#define XTAL_MASK_GM_MANUAL            ((u32)0x0000001F << 5)            /*!< R/W 0x11111  Gm manual at EN_XTAL_AAC_DIGI=0*/
#define XTAL_GM_MANUAL(x)              (((u32)((x) & 0x0000001F) << 5))
#define XTAL_GET_GM_MANUAL(x)          ((u32)(((x >> 5) & 0x0000001F)))
#define XTAL_BIT_EN_XTAL_AAC_GM        ((u32)0x00000001 << 4)            /*!< R/W 0x0  1: enable xtal aac gm control*/
#define XTAL_BIT_EN_XTAL_DRV_IQK_BCN   ((u32)0x00000001 << 3)            /*!< R/W 0x1  IQKPLL enable clock source control, 1:enable, 0: gate*/
#define XTAL_BIT_EN_XTAL_DRV_DIGI_DIV2 ((u32)0x00000001 << 2)            /*!< R/W 0x0  IQKPLL enable clock source control, 1:enable, 0: gate*/
#define XTAL_MASK_AAC_MODE             ((u32)0x00000003 << 0)            /*!< R/W 0x01  AAC_MODE for LPS 00: all XOXI cap off 01: cap from XTAL_SC_LPS 10: cap from XTAL_SC_INIT 11: cap from XTAL_SC_XI/XO*/
#define XTAL_AAC_MODE(x)               (((u32)((x) & 0x00000003) << 0))
#define XTAL_GET_AAC_MODE(x)           ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_OFF_0
  * @brief
  * @{
  */
#define XTAL_BIT_PDCK_OK_MANU      ((u32)0x00000001 << 31)           /*!< R/W 0x1  1: force PDCK_OK=1 for PDCK manual*/
#define XTAL_BIT_EN_XTAL_PDCK_VREF ((u32)0x00000001 << 30)           /*!< R/W 0x1  1: analog AAC VREF from PDCK digital circuit 0: analog AAC VREF from XTAL_VREF_SEL for BT only mode*/
#define XTAL_BIT_SEL_PWR           ((u32)0x00000001 << 29)           /*!< R/W 0x0  1: external 0.9V, 0: internal 0.9V (FIB)*/
#define XTAL_BIT_PK_SEL_OFFSET     ((u32)0x00000001 << 28)           /*!< R/W 0x0  1: XI Vpk-to-Vpk offset to 00: 0.4V  01: 0.5V  10: 0.6V  11: 0.7V*/
#define XTAL_MASK_MANU_PK_SEL      ((u32)0x00000003 << 26)           /*!< R/W 0x11  XI Vpk-to-Vpk 00: 0.7V  01: 0.8V  10: 0.9V  11: 1V*/
#define XTAL_MANU_PK_SEL(x)        (((u32)((x) & 0x00000003) << 26))
#define XTAL_GET_MANU_PK_SEL(x)    ((u32)(((x >> 26) & 0x00000003)))
#define XTAL_BIT_AACK_PK_MANU      ((u32)0x00000001 << 25)           /*!< R/W 0x0  1: manual for peak detector peak selection*/
#define XTAL_BIT_EN_XTAL_AAC_PKDET ((u32)0x00000001 << 24)           /*!< R/W 0x0  1: enable xtal aac peak detector*/
#define XTAL_BIT_LDO_OPVB_SEL      ((u32)0x00000001 << 22)           /*!< R/W 0x0  0: OP bias from IBG, 1: OP bias from Res divider*/
#define XTAL_BIT_LDO_NC            ((u32)0x00000001 << 21)           /*!< R/W 0x0  1: low current normal mode (22C) 0:high current normal mode (此pin要拉給BT控制)*/
#define XTAL_BIT_LPMODE            ((u32)0x00000001 << 20)           /*!< R/W 0x0  1: low power mode enable  22C turn off replica path, RF1 no CLK RF2 sweep to LPS buf*/
#define XTAL_BIT_DELAY_DIGI        ((u32)0x00000001 << 19)           /*!< R/W 0x1  1: turn on XI/XO Cfix, single side 8pF*/
#define XTAL_BIT_DELAY_USB         ((u32)0x00000001 << 18)           /*!< R/W 0x1  0: short, 1:long*/
#define XTAL_BIT_DELAY_AFE         ((u32)0x00000001 << 17)           /*!< R/W 0x1  0: short, 1:long*/
#define XTAL_MASK_DRV_DIGI         ((u32)0x00000003 << 15)           /*!< R/W 0x11  Driving control of DIGI/LPS clock buffer, 11:large current, 00: small current*/
#define XTAL_DRV_DIGI(x)           (((u32)((x) & 0x00000003) << 15))
#define XTAL_GET_DRV_DIGI(x)       ((u32)(((x >> 15) & 0x00000003)))
#define XTAL_MASK_DRV_USB          ((u32)0x00000003 << 13)           /*!< R/W 0x11  Driving control of USB clock buffer, 11:large current, 00: small current*/
#define XTAL_DRV_USB(x)            (((u32)((x) & 0x00000003) << 13))
#define XTAL_GET_DRV_USB(x)        ((u32)(((x >> 13) & 0x00000003)))
#define XTAL_MASK_DRV_AFE          ((u32)0x00000003 << 11)           /*!< R/W 0x11  Driving control of AFE clock buffer, 11:large current, 00: small current*/
#define XTAL_DRV_AFE(x)            (((u32)((x) & 0x00000003) << 11))
#define XTAL_GET_DRV_AFE(x)        ((u32)(((x >> 11) & 0x00000003)))
#define XTAL_MASK_DRV_RF2          ((u32)0x00000003 << 9)            /*!< R/W 0x11  Driving control of RF2 clock buffer, 11:large current, 00: small current*/
#define XTAL_DRV_RF2(x)            (((u32)((x) & 0x00000003) << 9))
#define XTAL_GET_DRV_RF2(x)        ((u32)(((x >> 9) & 0x00000003)))
#define XTAL_MASK_DRV_RF1          ((u32)0x00000003 << 7)            /*!< R/W 0x11  Dummy 由RFC控制*/
#define XTAL_DRV_RF1(x)            (((u32)((x) & 0x00000003) << 7))
#define XTAL_GET_DRV_RF1(x)        ((u32)(((x >> 7) & 0x00000003)))
#define XTAL_BIT_DRV_RF_LATCH      ((u32)0x00000001 << 6)            /*!< R/W 0x0  1: latch enable*/
#define XTAL_BIT_GM_SEP            ((u32)0x00000001 << 5)            /*!< R/W 0x0  0: GMP=GMN*/
#define XTAL_BIT_XQSEL_RF_AWAKE    ((u32)0x00000001 << 4)            /*!< R/W 0x1  0: Schmitt trigger, 1:replica INV(for OK0=1)*/
#define XTAL_BIT_XQSEL_RF_INITIAL  ((u32)0x00000001 << 3)            /*!< R/W 0x0  0: Schmitt trigger, 1:replica INV(for OK0=0) 22C OK0=0, RF no CLK*/
#define XTAL_BIT_XQSEL             ((u32)0x00000001 << 2)            /*!< R/W 0x0  0: Schmitt trigger, 1:replica INV(for AFE)*/
#define XTAL_BIT_GATED_XTAL_OK0    ((u32)0x00000001 << 1)            /*!< R/W 0x0  1:Gated xtal OK0*/
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_OFF_1
  * @brief
  * @{
  */
#define XTAL_BIT_EN_LNBUF       ((u32)0x00000001 << 10)          /*!< R/W 0x0  0: separate bias buffer (lower current higher noise) 1: self-bias (higher current lower noise)*/
#define XTAL_BIT_AAC_TIE_MID    ((u32)0x00000001 << 9)           /*!< R/W 0x0  XTAL_ AAC_TIE_MID=1 & EN_XTAL_PDCK_VREF=0 & using XTAL_VREF_SEL<4:0> for lock detect*/
#define XTAL_MASK_AAC_IOFFSET   ((u32)0x00000003 << 7)           /*!< R/W 0x00  Analog AAC IOFFSET current 00: off, 01:2uA, 10:4uA, 11:6uA*/
#define XTAL_AAC_IOFFSET(x)     (((u32)((x) & 0x00000003) << 7))
#define XTAL_GET_AAC_IOFFSET(x) ((u32)(((x >> 7) & 0x00000003)))
#define XTAL_MASK_AAC_CAP       ((u32)0x00000003 << 5)           /*!< R/W 0x11  Analog AAC domain pole cap 3pF/step 00: no cap,11: max cap*/
#define XTAL_AAC_CAP(x)         (((u32)((x) & 0x00000003) << 5))
#define XTAL_GET_AAC_CAP(x)     ((u32)(((x >> 5) & 0x00000003)))
#define XTAL_MASK_PDSW          ((u32)0x00000003 << 3)           /*!< R/W 0x01  XTAL analog AAC peak detector source follower size, 00: small 11: large*/
#define XTAL_PDSW(x)            (((u32)((x) & 0x00000003) << 3))
#define XTAL_GET_PDSW(x)        ((u32)(((x >> 3) & 0x00000003)))
#define XTAL_MASK_LPS_BUF_VB    ((u32)0x00000003 << 1)           /*!< R/W 0x00  00: small current, 11: large current*/
#define XTAL_LPS_BUF_VB(x)      (((u32)((x) & 0x00000003) << 1))
#define XTAL_GET_LPS_BUF_VB(x)  ((u32)(((x >> 1) & 0x00000003)))
#define XTAL_BIT_PDCK_MANU      ((u32)0x00000001 << 0)           /*!< R/W 0x0  1: open analog AAC loop for PDCK manual*/
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_OFF_2
  * @brief
  * @{
  */
#define XTAL_MASK_OFF_DUMMY1    ((u32)0x0000003F << 4)           /*!< R/W 0x0  HW used only*/
#define XTAL_OFF_DUMMY1(x)      (((u32)((x) & 0x0000003F) << 4))
#define XTAL_GET_OFF_DUMMY1(x)  ((u32)(((x >> 4) & 0x0000003F)))
#define XTAL_MASK_BG_T_OPTION   ((u32)0x00000007 << 1)           /*!< R/W 0x11  BG options*/
#define XTAL_BG_T_OPTION(x)     (((u32)((x) & 0x00000007) << 1))
#define XTAL_GET_BG_T_OPTION(x) ((u32)(((x >> 1) & 0x00000007)))
#define XTAL_BIT_OFF_DUMMY0     ((u32)0x00000001 << 0)           /*!< R/W 0x0  HW used only*/
/** @} */

/** @defgroup XTAL_FEN
  * @brief
  * @{
  */
#define XTAL_BIT_FEN_XMDDC ((u32)0x00000001 << 2) /*!< R/W 0x1  1:enable xmddc 0:reset xmddc module*/
#define XTAL_BIT_FEN_XAACK ((u32)0x00000001 << 1) /*!< R/W 0x0  1:enable AACK  0:reset AACK module*/
#define XTAL_BIT_FEN_XPDCK ((u32)0x00000001 << 0) /*!< R/W 0x0  1:enable PDCK  0:reset PDCK module*/
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_PDCK
  * @brief
  * @{
  */
#define XTAL_MASK_PDCK_STATE       ((u32)0x0000000F << 28)           /*!< R 0x0  PDCK state for debug*/
#define XTAL_PDCK_STATE(x)         (((u32)((x) & 0x0000000F) << 28))
#define XTAL_GET_PDCK_STATE(x)     ((u32)(((x >> 28) & 0x0000000F)))
#define XTAL_BIT_PDCK_LPOW         ((u32)0x00000001 << 23)           /*!< R/W 0x0  PDCK low power mode, 1:gating input clk Set to 1 when PDCK finished*/
#define XTAL_MASK_XPDCK_VREF_SEL   ((u32)0x0000001F << 18)           /*!< R/W 0x00011  PDCK calibrated result +offset  For AAC robust +3*/
#define XTAL_XPDCK_VREF_SEL(x)     (((u32)((x) & 0x0000001F) << 18))
#define XTAL_GET_XPDCK_VREF_SEL(x) ((u32)(((x >> 18) & 0x0000001F)))
#define XTAL_MASK_PDCK_UNIT        ((u32)0x00000003 << 16)           /*!< R/W 0x01  PDCK settling cycles (4M CLK) 00: 4us 01: 8us  10: 16us  11: 32us*/
#define XTAL_PDCK_UNIT(x)          (((u32)((x) & 0x00000003) << 16))
#define XTAL_GET_PDCK_UNIT(x)      ((u32)(((x >> 16) & 0x00000003)))
#define XTAL_MASK_VREF_INIT        ((u32)0x0000001F << 11)           /*!< R/W 0x11111  PDCK VREF initial setting*/
#define XTAL_VREF_INIT(x)          (((u32)((x) & 0x0000001F) << 11))
#define XTAL_GET_VREF_INIT(x)      ((u32)(((x >> 11) & 0x0000001F)))
#define XTAL_MASK_VREF_MANUAL      ((u32)0x0000001F << 6)            /*!< R/W/ES 0x11111  VREF manual at EN_XTAL_PDCK_DIGI=0 When pdck done, it will auto set this value to pdck result*/
#define XTAL_VREF_MANUAL(x)        (((u32)((x) & 0x0000001F) << 6))
#define XTAL_GET_VREF_MANUAL(x)    ((u32)(((x >> 6) & 0x0000001F)))
#define XTAL_MASK_PDCK_WAIT_CYC    ((u32)0x00000003 << 4)            /*!< R/W 0x01  Enable PDCK waiting (4M CLK) 00: 64us 01:128us  10:256us  11:512us*/
#define XTAL_PDCK_WAIT_CYC(x)      (((u32)((x) & 0x00000003) << 4))
#define XTAL_GET_PDCK_WAIT_CYC(x)  ((u32)(((x >> 4) & 0x00000003)))
#define XTAL_BIT_PDCK_SEARCH_MODE  ((u32)0x00000001 << 3)            /*!< R/W 0x0  0: binary search, 1: full search from 31*/
#define XTAL_BIT_EN_XTAL_PDCK_DIGI ((u32)0x00000001 << 2)            /*!< R/W 0x0  Default set 0 due to POW_XTAL will 1 before do PDCK*/
#define XTAL_BIT_RESET_N_PDCK      ((u32)0x00000001 << 1)            /*!< R/W 0x0  PDCK digital circuit reset (low to high),not used*/
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_AAC
  * @brief
  * @{
  */
#define XTAL_MASK_XAACK_AAC_MODE   ((u32)0x00000003 << 30)           /*!< R/W 0x11  11: XTAL_AACK_PK_SEL=<11>'s XTAL_GM_OUT3 10: XTAL_AACK_PK_SEL=<10>'s XTAL_GM_OUT2 01: XTAL_AACK_PK_SEL=<01>'s XTAL_GM_OUT1 00: XTAL_AACK_PK_SEL=<00>'s XTAL_GM_OUT0*/
#define XTAL_XAACK_AAC_MODE(x)     (((u32)((x) & 0x00000003) << 30))
#define XTAL_GET_XAACK_AAC_MODE(x) ((u32)(((x >> 30) & 0x00000003)))
#define XTAL_BIT_EN_XTAL_AAC_TRIG  ((u32)0x00000001 << 29)           /*!< R/W 0x0  MAC內部訊號(目前floating) Set by pmc and clr by HW finish calibration*/
#define XTAL_BIT_EN_XTAL_AAC       ((u32)0x00000001 << 28)           /*!< R/W 0x0  MAC內部訊號(目前floating) 0: 不做AAC (default), 1: power on 做AAC*/
#define XTAL_BIT_EN_XTAL_AAC_DIGI  ((u32)0x00000001 << 27)           /*!< R/W 0x0  1:enable aac digital circuit*/
#define XTAL_MASK_GM_STUP          ((u32)0x0000001F << 17)           /*!< R/W 0x11111  aac gm start up setting (pkdet=0時,回到最大gm)*/
#define XTAL_GM_STUP(x)            (((u32)((x) & 0x0000001F) << 17))
#define XTAL_GET_GM_STUP(x)        ((u32)(((x >> 17) & 0x0000001F)))
#define XTAL_MASK_CK_SET           ((u32)0x00000007 << 14)           /*!< R/W 0x011  aac settling cycles (4M CLK) 000:  64us 001:  128us 010:  256us 011:  512us 100:  1024us 101:  2048us 110:  4096us 111:  8192us*/
#define XTAL_CK_SET(x)             (((u32)((x) & 0x00000007) << 14))
#define XTAL_GET_CK_SET(x)         ((u32)(((x >> 14) & 0x00000007)))
#define XTAL_MASK_GM_INIT          ((u32)0x0000001F << 9)            /*!< R/W 0x11111  aac gm initial setting*/
#define XTAL_GM_INIT(x)            (((u32)((x) & 0x0000001F) << 9))
#define XTAL_GET_GM_INIT(x)        ((u32)(((x >> 9) & 0x0000001F)))
#define XTAL_BIT_GM_STEP           ((u32)0x00000001 << 8)            /*!< R/W 0x0  aac gm searching step. 0:step 1, 1:step 2*/
#define XTAL_MASK_XAAC_GM_OFFSET   ((u32)0x0000001F << 3)            /*!< R/W 0x00000  aac calibrated gm result +/- offset*/
#define XTAL_XAAC_GM_OFFSET(x)     (((u32)((x) & 0x0000001F) << 3))
#define XTAL_GET_XAAC_GM_OFFSET(x) ((u32)(((x >> 3) & 0x0000001F)))
#define XTAL_BIT_OFFSET_PLUS       ((u32)0x00000001 << 2)            /*!< R/W 0x1  0: aac calibrated gm result -offset , 1:aac calibrated gm result + offset*/
#define XTAL_BIT_RESET_N_AAC       ((u32)0x00000001 << 1)            /*!< R/W 0x0  AAC digital circuit reset (low to high) Default=0, 有EN_AAC 才會放開 (not used)*/
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_AAC_ON_1
  * @brief
  * @{
  */
#define XTAL_MASK_XAAC_DONE         ((u32)0x0000FFFF << 16)           /*!< R   for XAAC debug*/
#define XTAL_XAAC_DONE(x)           (((u32)((x) & 0x0000FFFF) << 16))
#define XTAL_GET_XAAC_DONE(x)       ((u32)(((x >> 16) & 0x0000FFFF)))
#define XTAL_MASK_XAAC_STATE        ((u32)0x0000000F << 12)           /*!< R   for XAAC debug*/
#define XTAL_XAAC_STATE(x)          (((u32)((x) & 0x0000000F) << 12))
#define XTAL_GET_XAAC_STATE(x)      ((u32)(((x >> 12) & 0x0000000F)))
#define XTAL_MASK_XAAC_DBG          ((u32)0x00000007 << 9)            /*!< R   for XAAC debug*/
#define XTAL_XAAC_DBG(x)            (((u32)((x) & 0x00000007) << 9))
#define XTAL_GET_XAAC_DBG(x)        ((u32)(((x >> 9) & 0x00000007)))
#define XTAL_BIT_XAAC_BUSY          ((u32)0x00000001 << 8)            /*!< R   for XAAC debug*/
#define XTAL_MASK_PK_END_AR         ((u32)0x00000003 << 4)            /*!< R/W 0x11  AAC peak sel end (star>end)*/
#define XTAL_PK_END_AR(x)           (((u32)((x) & 0x00000003) << 4))
#define XTAL_GET_PK_END_AR(x)       ((u32)(((x >> 4) & 0x00000003)))
#define XTAL_MASK_PK_START_AR       ((u32)0x00000003 << 2)            /*!< R/W 0x11  AACK peak sel start (star>end)*/
#define XTAL_PK_START_AR(x)         (((u32)((x) & 0x00000003) << 2))
#define XTAL_GET_PK_START_AR(x)     ((u32)(((x >> 2) & 0x00000003)))
#define XTAL_BIT_XAAC_LUT_MANUAL_EN ((u32)0x00000001 << 1)            /*!< R/W 0x0  1: enable LUT data: GM_MANUAL<4:0> , sel: AAC_MODE<1:0>*/
#define XTAL_BIT_XAAC_LPOW          ((u32)0x00000001 << 0)            /*!< R/W 0x0  AACK low power mode, 1:gating input clk*/
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_MODE_DEC_ON_0
  * @brief
  * @{
  */
#define XTAL_MASK_LPS_CAP_STEP   ((u32)0x00000003 << 30)           /*!< R/W 0x10  00:1 step/01:2 step/10:4 step /11:8 step 一次降幾檔Cap*/
#define XTAL_LPS_CAP_STEP(x)     (((u32)((x) & 0x00000003) << 30))
#define XTAL_GET_LPS_CAP_STEP(x) ((u32)(((x >> 30) & 0x00000003)))
#define XTAL_MASK_LPS_CAP_CYC    ((u32)0x00000003 << 28)           /*!< R/W 0x01  00:8u/01:16u/10:32u/11:64u 多久降一次Cap step*/
#define XTAL_LPS_CAP_CYC(x)      (((u32)((x) & 0x00000003) << 28))
#define XTAL_GET_LPS_CAP_CYC(x)  ((u32)(((x >> 28) & 0x00000003)))
#define XTAL_MASK_XMD_SEL        ((u32)0x00000003 << 26)           /*!< R/W 0x10  00:100K/01:500K/10:4M/11:12M 隨著系統頻率變化,需要改變此reg*/
#define XTAL_XMD_SEL(x)          (((u32)((x) & 0x00000003) << 26))
#define XTAL_GET_XMD_SEL(x)      ((u32)(((x >> 26) & 0x00000003)))
#define XTAL_BIT_SCAN_MODE_XMD   ((u32)0x00000001 << 25)           /*!< R/W 0x0  1:scan mode*/
#define XTAL_MASK_LDO_LPS        ((u32)0x00000007 << 22)           /*!< R/W 0x010  (起震後, 進入LPS時) 111: 1.1V  110: 1.05V  101:1V  100:0.95V 011: 0.9V  010: 0.85V   001: 0.8V  000: 0.75V*/
#define XTAL_LDO_LPS(x)          (((u32)((x) & 0x00000007) << 22))
#define XTAL_GET_LDO_LPS(x)      ((u32)(((x >> 22) & 0x00000007)))
#define XTAL_MASK_WAIT_CYC       ((u32)0x0000003F << 16)           /*!< R/W 0x000011  Next state wait cycle, 83ns/unit*/
#define XTAL_WAIT_CYC(x)         (((u32)((x) & 0x0000003F) << 16))
#define XTAL_GET_WAIT_CYC(x)     ((u32)(((x >> 16) & 0x0000003F)))
#define XTAL_MASK_LDO_OK         ((u32)0x00000007 << 13)           /*!< R/W 0x011  (起震後) 111: 1.1V  110: 1.05V  101:1V  100:0.95V 011: 0.9V  010: 0.85V   001: 0.8V  000: 0.75V*/
#define XTAL_LDO_OK(x)           (((u32)((x) & 0x00000007) << 13))
#define XTAL_GET_LDO_OK(x)       ((u32)(((x >> 13) & 0x00000007)))
#define XTAL_BIT_MD_LPOW         ((u32)0x00000001 << 12)           /*!< R/W 0x0  XTAL mode decoder low power mode 1: gated input clock*/
#define XTAL_MASK_OV_RATIO       ((u32)0x00000003 << 10)           /*!< R/W 0x01  overlap unit times (4M CLK) 00:  128us 01:  256us 10:  512us 11:  1024us*/
#define XTAL_OV_RATIO(x)         (((u32)((x) & 0x00000003) << 10))
#define XTAL_GET_OV_RATIO(x)     ((u32)(((x >> 10) & 0x00000003)))
#define XTAL_MASK_OV_UNIT        ((u32)0x00000007 << 7)            /*!< R/W 0x1  overlap time settling cycles (4M CLK)  000: 10us 001: 20us  010: 40us 011: 80us 100: 160us 101: 320us 110: 640us 111: 1280us*/
#define XTAL_OV_UNIT(x)          (((u32)((x) & 0x00000007) << 7))
#define XTAL_GET_OV_UNIT(x)      ((u32)(((x >> 7) & 0x00000007)))
#define XTAL_MASK_MODE_MANUAL    ((u32)0x00000003 << 5)            /*!< R/W 0x11  01: LPS mode 10: Normal mode 11: high performance mode [efuse for power on ]*/
#define XTAL_MODE_MANUAL(x)      (((u32)((x) & 0x00000003) << 5))
#define XTAL_GET_MODE_MANUAL(x)  ((u32)(((x >> 5) & 0x00000003)))
#define XTAL_BIT_MANU_SEL        ((u32)0x00000001 << 4)            /*!< R/W 0x0  1: enable xtal mode manual [efuse for power on ]*/
#define XTAL_BIT_POW_XTAL_LPS    ((u32)0x00000001 << 3)            /*!< R 0x0  POW_XTAL_LPS*/
#define XTAL_BIT_MODE            ((u32)0x00000001 << 2)            /*!< R 0x0  22C xtal mode define: {POW_XTAL, XTAL_MODE, POW_XTAL_LPS} [efuse for power on ]*/
#define XTAL_BIT_RESET_N_XMD     ((u32)0x00000001 << 1)            /*!< R/W 0x1  Decoder digital circuit reset (low to high) Default 1&POR 再給reset_n. (not used)*/
/** @} */

/** @defgroup XTAL_ANAPAR_XTAL_MODE_DEC_ON_1
  * @brief
  * @{
  */
#define XTAL_MASK_OV_RATIO_FST   ((u32)0x00000003 << 30)           /*!< R/W 0x0  overlap unit times (4M CLK), used for HP2NM or NM2HP 00:  128us 01:  256us 10:  512us 11:  1024us*/
#define XTAL_OV_RATIO_FST(x)     (((u32)((x) & 0x00000003) << 30))
#define XTAL_GET_OV_RATIO_FST(x) ((u32)(((x >> 30) & 0x00000003)))
#define XTAL_MASK_XMD_STATE      ((u32)0x0000001F << 16)           /*!< R   for xmd debug*/
#define XTAL_XMD_STATE(x)        (((u32)((x) & 0x0000001F) << 16))
#define XTAL_GET_XMD_STATE(x)    ((u32)(((x >> 16) & 0x0000001F)))
#define XTAL_MASK_MODE_DEC       ((u32)0x00000003 << 8)            /*!< R   for xmd debug 10:NM 01: LPS 11: HP*/
#define XTAL_MODE_DEC(x)         (((u32)((x) & 0x00000003) << 8))
#define XTAL_GET_MODE_DEC(x)     ((u32)(((x >> 8) & 0x00000003)))
#define XTAL_BIT_LPS_CAP_CTRL    ((u32)0x00000001 << 0)            /*!< R/W 0x1  1:cap step control*/
/** @} */

/** @defgroup XTAL_DEBUG
  * @brief
  * @{
  */
#define XTAL_MASK_TOUT             ((u32)0x000003FF << 22)           /*!< R/W 0x1111111111  xtal arbiter wait feedback signal timeout delay cycle, if this valua is max, arbiter will disable timeout function. Time unit is xtal_tutime*/
#define XTAL_TOUT(x)               (((u32)((x) & 0x000003FF) << 22))
#define XTAL_GET_TOUT(x)           ((u32)(((x >> 22) & 0x000003FF)))
#define XTAL_MASK_XAAC_PK_SEL      ((u32)0x00000003 << 19)           /*!< R   XAAC_PK_SEL(AAC calibration address)*/
#define XTAL_XAAC_PK_SEL(x)        (((u32)((x) & 0x00000003) << 19))
#define XTAL_GET_XAAC_PK_SEL(x)    ((u32)(((x >> 19) & 0x00000003)))
#define XTAL_BIT_XAAC_READY        ((u32)0x00000001 << 18)           /*!< R   signal for xtal*/
#define XTAL_MASK_GM_OUT           ((u32)0x0000001F << 13)           /*!< R   AACK digital calculate result for ANA comparator*/
#define XTAL_GM_OUT(x)             (((u32)((x) & 0x0000001F) << 13))
#define XTAL_GET_GM_OUT(x)         ((u32)(((x >> 13) & 0x0000001F)))
#define XTAL_BIT_XPDCK_BUSY        ((u32)0x00000001 << 12)           /*!< R   signal for xtal*/
#define XTAL_BIT_XPDCK_READY       ((u32)0x00000001 << 11)           /*!< R   signal for xtal*/
#define XTAL_MASK_XPDCK_VREF_OUT   ((u32)0x0000001F << 6)            /*!< R   PDCK digital calculate result for ANA comparator*/
#define XTAL_XPDCK_VREF_OUT(x)     (((u32)((x) & 0x0000001F) << 6))
#define XTAL_GET_XPDCK_VREF_OUT(x) ((u32)(((x >> 6) & 0x0000001F)))
#define XTAL_BIT_MANU_POW_BG       ((u32)0x00000001 << 5)            /*!< R/W 0x0  manual mode POW_BG*/
#define XTAL_BIT_MANU_XTAL_MODE    ((u32)0x00000001 << 4)            /*!< R/W 0x0  manual mode XTAL_MODE*/
#define XTAL_BIT_MANU_POW_XTAL_LPS ((u32)0x00000001 << 3)            /*!< R/W 0x0  manual mode POW_XTAL_LPS*/
#define XTAL_BIT_MANU_POW_XTAL     ((u32)0x00000001 << 2)            /*!< R/W 0x0  manual mode POW_XTAL*/
#define XTAL_BIT_AUTO_PDCK         ((u32)0x00000001 << 1)            /*!< R/W/ES 0x1  Auto PDCK enable signal. If set this bit, arbiter will do PDCK before XTAL enter LPS mode. Once PDCK done , this bit will auto set 0*/
#define XTAL_BIT_ARB_MANUAL_EN     ((u32)0x00000001 << 0)            /*!< R/W 0x0  Aribter manual mode enable*/
/** @} */

/** @defgroup XTAL_LPS_FREQ_NM
  * @brief
  * @{
  */
#define XTAL_MASK_CLK_FREQ_NM   ((u32)0x0007FFFF << 0)           /*!< R/W 19'h4c4b4  xtal 312.5K freq in normal mode , all cap are valid , it is exactly 312.5K.*/
#define XTAL_CLK_FREQ_NM(x)     (((u32)((x) & 0x0007FFFF) << 0))
#define XTAL_GET_CLK_FREQ_NM(x) ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */

/** @defgroup XTAL_LPS_FREQ_LPS
  * @brief
  * @{
  */
#define XTAL_MASK_CLK_FREQ_LPS   ((u32)0x0007FFFF << 0)           /*!< R/W 19'h4c4b4  xtal 312.5K freq in lps mode , cap maybe disable for power save , maximumly , freq will be 312.5K + 250ppm*/
#define XTAL_CLK_FREQ_LPS(x)     (((u32)((x) & 0x0007FFFF) << 0))
#define XTAL_GET_CLK_FREQ_LPS(x) ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */

/** @defgroup XTAL_LPS_SDM0
  * @brief
  * @{
  */
#define XTAL_BIT_SDM0_EN      ((u32)0x00000001 << 16)          /*!< R/W 1'b1  xtal lps sdm0 output enable Note : sdm0 used for system and wifi, default is 32768Hz.*/
#define XTAL_MASK_SDM0_FREQ   ((u32)0x0000FFFF << 0)           /*!< R/W 16'h8000  xtal lps sdm0 output frequency, unit is Hz. Note : sdm0 used for system and wifi, default is 32768Hz.*/
#define XTAL_SDM0_FREQ(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define XTAL_GET_SDM0_FREQ(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup XTAL_LPS_SDM1
  * @brief
  * @{
  */
#define XTAL_BIT_SDM1_EN      ((u32)0x00000001 << 16)          /*!< R/W 1'b1  xtal lps sdm1 output enable Note : sdm1 used for bt, default is 32000Hz.*/
#define XTAL_MASK_SDM1_FREQ   ((u32)0x0000FFFF << 0)           /*!< R/W 16'h7d00  xtal lps sdm1 output frequency, unit is Hz. Note : sdm1 used for system and wifi, default is 32000Hz.*/
#define XTAL_SDM1_FREQ(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define XTAL_GET_SDM1_FREQ(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup XTAL_Exported_Types XTAL Exported Types
  * @{
  */

/** @brief XTAL Register Declaration
  */

typedef struct {
	__IO uint32_t XTAL_ANAPAR_POW_XTAL_0        ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t XTAL_ANAPAR_XTAL_ON_0         ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t XTAL_ANAPAR_XTAL_ON_1         ;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t XTAL_ANAPAR_XTAL_ON_2         ;  /*!< Register,  Address offset:0x00C */
	__IO uint32_t XTAL_ANAPAR_XTAL_OFF_0        ;  /*!< Register,  Address offset:0x010 */
	__IO uint32_t XTAL_ANAPAR_XTAL_OFF_1        ;  /*!< Register,  Address offset:0x014 */
	__IO uint32_t XTAL_ANAPAR_XTAL_OFF_2        ;  /*!< Register,  Address offset:0x018 */
	__IO uint32_t XTAL_FEN                      ;  /*!< Register,  Address offset:0x01C */
	__IO uint32_t XTAL_ANAPAR_XTAL_PDCK         ;  /*!< Register,  Address offset:0x020 */
	__IO uint32_t XTAL_ANAPAR_XTAL_AAC          ;  /*!< Register,  Address offset:0x024 */
	__IO uint32_t XTAL_ANAPAR_XTAL_AAC_ON_1     ;  /*!< Register,  Address offset:0x028 */
	__IO uint32_t XTAL_ANAPAR_XTAL_MODE_DEC_ON_0;  /*!< Register,  Address offset:0x02C */
	__IO uint32_t XTAL_ANAPAR_XTAL_MODE_DEC_ON_1;  /*!< Register,  Address offset:0x030 */
	__IO uint32_t XTAL_DEBUG                    ;  /*!< Register,  Address offset:0x034 */
	__IO uint32_t XTAL_ART_CKON_TIM             ;  /*!< Register,  Address offset:0x038 */
	__IO uint32_t RSVD0                         ;  /*!< Reserved,  Address offset:0x03C-0x03F */
	__IO uint32_t XTAL_LPS_FREQ_NM              ;  /*!< Register,  Address offset:0x040 */
	__IO uint32_t XTAL_LPS_FREQ_LPS             ;  /*!< Register,  Address offset:0x044 */
	__IO uint32_t XTAL_LPS_SDM0                 ;  /*!< Register,  Address offset:0x048 */
	__IO uint32_t XTAL_LPS_SDM1                 ;  /*!< Register,  Address offset:0x04C */
} XTAL_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here
/** @defgroup XTAL_CLOCK_MODE_define
  * @{
  */
enum xtal_mode_sleep {
	XTAL_OFF = 0,
	XTAL_LPS_Without_40M = 1,
	XTAL_LPS_With_40M = 2,
	XTAL_Normal = 3,
	XTAL_HP = 4,
};
/** @} */
/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */