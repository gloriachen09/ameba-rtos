/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"
#include "usb_hal.h"

/* Private defines -----------------------------------------------------------*/

/* USB OTG addon control register */
#define USB_ADDON_REG_CTRL							(USB_ADDON_REG_BASE + 0x04UL)

#define USB_ADDON_REG_CTRL_BIT_UPLL_CKRDY			BIT(5)  /* 1: USB PHY clock ready */
#define USB_ADDON_REG_CTRL_BIT_USB_OTG_RST			BIT(8)  /* 1: Enable USB OTG */
#define USB_ADDON_REG_CTRL_BIT_USB_DPHY_FEN			BIT(9)  /* 1: Enable USB DPHY */
#define USB_ADDON_REG_CTRL_BIT_USB_APHY_EN			BIT(14) /* 1: Enable USB APHY */
#define USB_ADDON_REG_CTRL_BIT_LS_HST_UTMI_EN		BIT(22) /* 1: Enable the support of low-speed host mode when using utmi 16bit */
#define USB_ADDON_REG_CTRL_BIT_HS_IP_GAP_OPT_POS	20U		/* MAC high-speed host inter-packet delay */
#define USB_ADDON_REG_CTRL_BIT_HS_IP_GAP_OPT_MASK	(0x3U << USB_ADDON_REG_CTRL_BIT_HS_IP_GAP_OPT_POS)
#define USB_ADDON_REG_CTRL_BIT_HS_IP_GAP_OPT		(USB_ADDON_REG_CTRL_BIT_HS_IP_GAP_OPT_MASK)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usb_chip_init(u8 mode);
static int usb_chip_deinit(void);
static usb_cal_data_t *usb_chip_get_cal_data(u8 mode);
static void usb_chip_enable_interrupt(u8 priority);
static void usb_chip_disable_interrupt(void);
static void usb_chip_register_irq_handler(void *handler, u8 priority);
static void usb_chip_unregister_irq_handler(void);

/* Private variables ---------------------------------------------------------*/

static const usb_cal_data_t usb_cal_data[] = {
	{0x00, 0xE0, 0x9D},
	{0x00, 0xE1, 0x19},
	{0x00, 0xE2, 0xDB},
	{0x00, 0xE4, 0x68}, // 0x6B
	{0x01, 0xE5, 0x0A},
	{0x01, 0xE6, 0xD8},
	{0x02, 0xE7, 0x52}, // 0x32
	{0x01, 0xE0, 0x04},
	{0x01, 0xE0, 0x00},
	{0x01, 0xE0, 0x04},

	{0xFF, 0x00, 0x00}
};

/* Exported variables --------------------------------------------------------*/

usb_hal_driver_t usb_hal_driver = {
	.init = usb_chip_init,
	.deinit = usb_chip_deinit,
	.get_cal_data = usb_chip_get_cal_data,
	.enable_interrupt = usb_chip_enable_interrupt,
	.disable_interrupt = usb_chip_disable_interrupt,
	.register_irq_handler = usb_chip_register_irq_handler,
	.unregister_irq_handler = usb_chip_unregister_irq_handler,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Get USB chip specific calibration data
  * @param  mode: 0 - device; 1 - host
  * @retval Pointer to calibration data buffer
  */
static usb_cal_data_t *usb_chip_get_cal_data(u8 mode)
{
	UNUSED(mode);
	return (usb_cal_data_t *)&usb_cal_data[0];
}

/**
  * @brief  USB chip specific initialization
  * @param  void
  * @retval Status
  */
static int usb_chip_init(u8 mode)
{
	UNUSED(mode);

	u32 reg = 0;
	u32 count = 0;

	/* Enable BG */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1);
	reg |= (LSYS_BIT_BG_PWR | LSYS_BIT_BG_ON_USB2);
	reg &= ~LSYS_MASK_BG_ALL;
	if (EFUSE_GetChipVersion() != SYSCFG_CUT_VERSION_A) {
		reg |= LSYS_BG_ALL(0x2);
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1, reg);

	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, ENABLE);

	/* USB Power Sequence */
	/* USB digital pad en,dp/dm sharing GPIO PAD */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg &= ~(HSYS_BIT_USB2_DIGOTGPADEN | HSYS_BIT_USB_OTGMODE | HSYS_BIT_USB2_DIGPADEN);
#ifdef CONFIG_USB_OTG_VERIFY
	reg |= (HSYS_BIT_USB_OTGMODE | HSYS_BIT_OTG_ANA_EN);
#endif
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	/* USB PWC_UALV_EN,  PWC_UAHV_EN */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg |= (HSYS_BIT_PWC_UALV_EN | HSYS_BIT_PWC_UAHV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);
	DelayUs(2);

	/* USB PWC_UABG_EN */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg |= HSYS_BIT_PWC_UABG_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);
	DelayUs(10);

	/* USB ISO_USBD_EN = 0 => disable isolation output signal from PD_USBD*/
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg &= ~HSYS_BIT_ISO_USBA_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);
	DelayUs(10);

	/* USB enable PHY */
	reg = HAL_READ32(USB_ADDON_REG_CTRL, 0U);
	reg &= ~USB_ADDON_REG_CTRL_BIT_HS_IP_GAP_OPT_MASK;
	reg |= (0x3U << USB_ADDON_REG_CTRL_BIT_HS_IP_GAP_OPT_POS); // Inter-packet gap 343ns, spec 399ns
	reg |= (USB_ADDON_REG_CTRL_BIT_USB_DPHY_FEN | USB_ADDON_REG_CTRL_BIT_USB_APHY_EN | USB_ADDON_REG_CTRL_BIT_LS_HST_UTMI_EN);
	HAL_WRITE32(USB_ADDON_REG_CTRL, 0U, reg);
	DelayUs(34);

	/* Wait UPLL_CKRDY */
	do {
		/* 1ms timeout expected, 10ms for safe */
		DelayUs(10);
		if (++count > 1000U) {
			return HAL_TIMEOUT;
		}
	} while (!(HAL_READ32(USB_ADDON_REG_CTRL, 0U) & USB_ADDON_REG_CTRL_BIT_UPLL_CKRDY));

	/* USBOTG_EN = 1 => enable USBOTG */
	reg = HAL_READ32(USB_ADDON_REG_CTRL, 0U);
	reg |= USB_ADDON_REG_CTRL_BIT_USB_OTG_RST;
	HAL_WRITE32(USB_ADDON_REG_CTRL, 0U, reg);

	return HAL_OK;
}

/**
  * @brief  USB chip specific deinitialization
  * @param  void
  * @retval Status
  */
static int usb_chip_deinit(void)
{
	u32 reg = 0;

	/* USBOTG_EN = 0 => disable USBOTG */
	reg = HAL_READ32(USB_ADDON_REG_CTRL, 0U);
	reg &= (~USB_ADDON_REG_CTRL_BIT_USB_OTG_RST);
	HAL_WRITE32(USB_ADDON_REG_CTRL, 0U, reg);

	/* USBPHY_EN = 0 */
	reg = HAL_READ32(USB_ADDON_REG_CTRL, 0U);
	reg &= (~(USB_ADDON_REG_CTRL_BIT_USB_DPHY_FEN | USB_ADDON_REG_CTRL_BIT_USB_APHY_EN));
	HAL_WRITE32(USB_ADDON_REG_CTRL, 0U, reg);

	/* USB ISO_USBD_EN = 1 => enable isolation output signal from PD_USBD*/
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg |= HSYS_BIT_ISO_USBA_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	/* USB PWC_UABG_EN = 0 */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg &= ~HSYS_BIT_PWC_UABG_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	/* PWC_UPHV_EN  = 0 => disable USBPHY analog 3.3V power */
	/* PWC_UPLV_EN = 0 => disable USBPHY analog 1.2V power */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg &= ~(HSYS_BIT_PWC_UALV_EN | HSYS_BIT_PWC_UAHV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	/* USB digital pad disable*/
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg |= (HSYS_BIT_USB2_DIGOTGPADEN | HSYS_BIT_USB2_DIGPADEN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, DISABLE);

	/* Disable BG */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1);
	reg &= ~LSYS_BIT_BG_ON_USB2;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1, reg);

	return HAL_OK;
}

/**
  * @brief  Enable USB interrupt
  * @param  priority: IRQ priority
  * @retval void
  */
static void usb_chip_enable_interrupt(u8 priority)
{
	UNUSED(priority);
	InterruptEn(USB_OTG_IRQ, priority);
}

/**
  * @brief  Disable USB interrupt
  * @retval void
  */
static void usb_chip_disable_interrupt(void)
{
	InterruptDis(USB_OTG_IRQ);
}

/**
  * @brief  Register USB IRQ handler
  * @param  handler: IRQ handler
  * @param  priority: IRQ priority
  * @retval void
  */
static void usb_chip_register_irq_handler(void *handler, u8 priority)
{
	if (handler != NULL) {
		InterruptRegister((IRQ_FUN)handler, USB_OTG_IRQ, NULL, priority);
	}
}

/**
  * @brief  Unregister USB IRQ handler
  * @retval void
  */
static void usb_chip_unregister_irq_handler(void)
{
	InterruptUnRegister(USB_OTG_IRQ);
}

/* Exported functions --------------------------------------------------------*/
