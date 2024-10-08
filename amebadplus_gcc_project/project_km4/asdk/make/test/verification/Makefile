
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/verification
MODULE_IFLAGS = -I$(DIR)./
MODULE_IFLAGS += -I$(BASEDIR)/component/application/ir
CFGDIR = $(TARGETDIR)/usrcfg
vpath %.c $(DIR) $(CFGDIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
ifeq ($(CONFIG_ADC_VERIFY),y)
CSRC = $(DIR)/adc/ameba_adc_test.c
endif

ifeq ($(CONFIG_CAPTOUCH_VERIFY),y)
CSRC += $(DIR)/captouch/ameba_captouch_test.c
endif

ifeq ($(CONFIG_AF_VERIFY),y)
CSRC += $(DIR)/af/af_test.c
endif

ifeq ($(CONFIG_FT),y)
MODULE_IFLAGS += -I$(DIR)/ft_test/adc/
CSRC += $(DIR)/ft_test/adc/rtl8195a_ft_adc_dac.c
CSRC += $(DIR)/ft_test/adc/rtl8195a_dac_sin_23p84.c
CSRC += $(DIR)/ft_test/adc/rtl8195a_i2c_spi_dac.c
CSRC += $(DIR)/ft_test/adc/rtl8195a_spidac_sin_5p722k.c
CSRC += $(DIR)/ft_test/adc/rtl8710b_ft_test.c
endif

ifeq ($(CONFIG_EQC),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/common
MODULE_IFLAGS += -I$(DIR)/eqc
CSRC += $(DIR)/eqc/ameba_eqc.c
CSRC += $(DIR)/eqc/ameba_eqc_ext.c
endif

ifeq ($(CONFIG_QA),y)
MODULE_IFLAGS += -I$(DIR)/qa_test
CSRC += $(DIR)/qa_test/ameba_qa.c
CSRC += $(DIR)/qa_test/ameba_gpio_qa_test.c
CSRC += $(DIR)/qa_test/ameba_ft_adc_dac_qa_test.c
CSRC += $(DIR)/qa_test/ameba_ft_adc_dac_dmic_qa_test.c
CSRC += $(DIR)/qa_test/ameba_memory_qa_test.c
CSRC += $(DIR)/qa_test/ameba_wifi_qa_test.c
CSRC += $(DIR)/qa_test/ameba_adc_key_detect_qa_test.c
endif

ifeq ($(CONFIG_OLT),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/common
MODULE_IFLAGS += -I$(TARGETDIR)/fwlib/include
MODULE_IFLAGS += -I$(DIR)/olt_test
CSRC += $(DIR)/olt_test/ameba_olt.c
CSRC += $(DIR)/olt_test/ameba_olt_ext.c
endif

ifeq ($(CONFIG_EMC),y)
MODULE_IFLAGS += -I$(DIR)/emc
CSRC += $(DIR)/emc/ameba_emc.c
endif

ifeq ($(CONFIG_CornerTest),y)
MODULE_IFLAGS += -I$(DIR)/corner_test
MODULE_IFLAGS += -I$(TARGETDIR)/fwlib/include
CSRC += $(DIR)/corner_test/ameba_corner_test.c
CSRC += $(DIR)/corner_test/ameba_corner_test_ext.c
endif

ifeq ($(CONFIG_CRYPTO_VERIFY),y)
CSRC += $(DIR)/crypto/ameba_crypto_test.c
#CSRC += $(DIR)/crypto/cryptoSim.c
#CSRC += $(DIR)/crypto/ameba_aes_ecb_test.c
endif

ifeq ($(CONFIG_EFUSE_VERIFY),y)
CSRC += $(DIR)/efuse/ameba_efuse_test.c
endif

ifeq ($(CONFIG_GDMA_VERIFY),y)
CSRC += $(DIR)/gdma/ameba_gdma_test.c
CSRC += $(DIR)/gdma/ameba_gdma_llp_test.c
endif

ifeq ($(CONFIG_GPIO_VERIFY),y)
CSRC += $(DIR)/gpio/ameba_gpio_test.c
endif

ifeq ($(CONFIG_PINMUX_VERIFY),y)
CSRC += $(DIR)/pinmux/ameba_pinmux_test.c
endif

ifeq ($(CONFIG_I2C_VERIFY),y)
CSRC += $(DIR)/i2c/i2c_test.c
CSRC += $(DIR)/i2c/hal_i2c_test.c
endif

ifeq ($(CONFIG_PSRAM_VERIFY),y)
CSRC += $(DIR)/psram/psram_pressure_test.c
endif

ifeq ($(CONFIG_I2S_VERIFY),y)
CSRC += $(DIR)/i2s/rtl8195a_i2s_test.c
endif

ifeq ($(CONFIG_AUDIO_VERIFY),y)
CSRC += $(DIR)/audio/rtl8721d_audio_test.c
#CSRC += $(DIR)/audio/rtl8721d_audio_llp_test.c
#CSRC += $(DIR)/audio/rtl8721d_audio_cyclic_dma_test.c
#CSRC += $(DIR)/audio/rtl8721d_audio_opus_test.c
endif

ifeq ($(CONFIG_SDIO_DEVICE_VERIFY),y)
CSRC += $(DIR)/sdio_device/rtl8195a_sdio_device_test.c
endif

ifeq ($(CONFIG_SPI_COM_VERIFY),y)
CSRC += $(DIR)/spi_communication/rtl8721d_ssi_test.c
CSRC += $(DIR)/spi_communication/hal_ssi_test.c
endif

ifeq ($(CONFIG_SPIC_VERIFY),y)
#CSRC += $(DIR)/spi_flash/rtl8721d_spi_flash_test.c
#CSRC += $(DIR)/spi_flash/rtl8721d_spi_flash_qc_test.c
#CSRC += $(DIR)/spi_flash/flash_runcode_test.c
#CSRC += $(DIR)/spi_flash/rtl8721d_flash_N25Q00AA_patch.c

ifeq ($(CONFIG_MCM_MEM_VERIFY),y)
CSRC += $(DIR)/spi_flash/ameba_flash_test_GD25WQ64E.c
CSRC += $(DIR)/spi_flash/ameba_flash_test_PY25Q32HB.c
CSRC += $(DIR)/spi_flash/ameba_flash_test_ZG25LQ32B.c
CSRC += $(DIR)/spi_flash/ameba_psram_test_W955D8MKY.c
CSRC += $(DIR)/spi_flash/ameba_psram_test_APS6408L.c
endif
endif

ifeq ($(CONFIG_TIMER_VERIFY),y)
CSRC += $(DIR)/timer/ameba_timer_test.c
CSRC += $(DIR)/timer/ameba_rtc_test.c
CSRC += $(DIR)/timer/ameba_rtcio_test.c
CSRC += $(DIR)/timer/ameba_pwm_test.c
CSRC += $(DIR)/timer/ameba_timer_capture_test.c
endif

ifeq ($(CONFIG_PRS_VERIFY),y)
CSRC += $(DIR)/ups/ameba_ups_test.c
endif

ifeq ($(CONFIG_UART_VERIFY),y)
CSRC += $(DIR)/uart/ameba_uart_test.c
CSRC += $(DIR)/uart/hal_uart_test.c
CSRC += $(DIR)/uart/ameba_uart_cyclic_dma_test.c
endif

ifeq ($(CONFIG_LEDC_VERIFY),y)
MODULE_IFLAGS += -I$(DIR)/ledc
CSRC += $(DIR)/ledc/ameba_ledc_test.c
endif

ifeq ($(CONFIG_LCDC_VERIFY),y)
MODULE_IFLAGS += -I$(DIR)/lcdc/
MODULE_IFLAGS += -I$(DIR)/lcdc/demo_code
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/ucgui/Config/
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/ucgui/GUI/Core/
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/ucgui/GUI/JPEG/
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/ucgui/GUI/MultiLayer/
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/ucgui/GUI/Widget/
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/ucgui/GUI/WM/
CSRC += $(DIR)/lcdc/ameba_lcdc_test.c
CSRC += $(DIR)/lcdc/ameba_mipi_test.c
#CSRC += $(DIR)/lcdc/bsp_mcu_lcd.c
#CSRC += $(DIR)/lcdc/bsp_rgb_lcd.c
#CSRC += $(DIR)/lcdc/bsp_led_board.c
#CSRC += $(DIR)/lcdc/bsp_lcdc_font.c
#CSRC += $(DIR)/lcdc/gt9147.c
#CSRC += $(DIR)/lcdc/ctiic.c
#CSRC += $(DIR)/lcdc/touch.c

#CSRC += $(DIR)/lcdc/demo_code/clock_menu.c
#CSRC += $(DIR)/lcdc/demo_code/grad_menu.c
#CSRC += $(DIR)/lcdc/demo_code/gui_cus_parts.c
#CSRC += $(DIR)/lcdc/demo_code/light_menu.c
#CSRC += $(DIR)/lcdc/demo_code/main_menu.c
#CSRC += $(DIR)/lcdc/demo_code/realtek_demo.c
#CSRC += $(DIR)/lcdc/demo_code/start_menu.c
#CSRC += $(DIR)/lcdc/demo_code/tempre_menu.c
#CSRC += $(DIR)/lcdc/demo_code/wave_menu.c
#CSRC += $(DIR)/lcdc/demo_code/clock_pic.c
#CSRC += $(DIR)/lcdc/demo_code/gradienter_pic.c
#CSRC += $(DIR)/lcdc/demo_code/light_off_pic.c
#CSRC += $(DIR)/lcdc/demo_code/light_on_pic.c
#CSRC += $(DIR)/lcdc/demo_code/light_pic.c
#CSRC += $(DIR)/lcdc/demo_code/realtek_logo.c
#CSRC += $(DIR)/lcdc/demo_code/return_btn_pic.c
#CSRC += $(DIR)/lcdc/demo_code/tempre_pic.c
#CSRC += $(DIR)/lcdc/demo_code/wave_pic.c
endif

ifeq ($(CONFIG_WDG_VERIFY),y)
CSRC += $(DIR)/wdg/ameba_wdg_test.c
endif

ifeq ($(CONFIG_TRNG_VERIFY),y)
CSRC += $(DIR)/trng/ameba_trng_test.c
endif

ifeq ($(CONFIG_IPC_VERIFY),y)
CSRC += $(DIR)/ipc/ipc_test.c
endif

ifeq ($(CONFIG_MPU_VERIFY),y)
CSRC += $(DIR)/mpu/mpu_test.c
endif

ifeq ($(CONFIG_RSIP_VERIFY),y)
CSRC += $(DIR)/rsip/rsip_test.c
endif

ifeq ($(CONFIG_DHRYSTONE_TEST),y)
CSRC += $(DIR)/dhrystone/dhrystone_test.c
endif

ifeq ($(CONFIG_PERFORMANCE_VERIFY),y)
CSRC += $(DIR)/platform/ameba_performance_test.c
endif

ifeq ($(CONFIG_SBOOT_VERIFY),y)
CSRC += $(DIR)/sboot/sboot_test.c
endif

ifeq ($(CONFIG_CACHE_VERIFY),y)
CSRC += $(DIR)/cache/ameba_cache_test.c
endif

ifeq ($(CONFIG_SEC_VERIFY),y)
CSRC += $(DIR)/secure/secure_test_ns.c
#CSRC += $(DIR)/secure/cryptoSimsecure.c
endif

ifeq ($(CONFIG_WFE_VERIFY),y)
CSRC += $(DIR)/wfe/wfe_test.c
endif

ifeq ($(CONFIG_IR_VERIFY),y)
CSRC += $(DIR)/IR/ameba_ir_test.c
CSRC += $(DIR)/IR/ameba_ir_learn.c
CSRC += $(DIR)/IR/protocol/ir_nec_protocol.c
endif

ifeq ($(CONFIG_PMC_VERIFY),y)
CSRC += $(DIR)/pmc/pmc_test_km4.c
endif

ifeq ($(CONFIG_FTL_VERIFY),y)
CSRC += $(DIR)/ftl/rtl8721d_ftl_test.c
endif

ifeq ($(CONFIG_SDIOH_VERIFY),y)
CSRC += $(DIR)/sdio_host/rtl8721d_sdio_host_test.c
endif

ifeq ($(CONFIG_DDR_VERIFY),y)
CSRC += $(DIR)/ddr/iot_ddr_test.c
endif

ifeq ($(CONFIG_RSA_VERIFY),y)
CSRC += $(DIR)/rsa/rsa_test.c
endif

ifeq ($(CONFIG_ECDSA_VERIFY),y)
CSRC += $(DIR)/ecdsa/ecdsa_test.c
endif

ifeq ($(CONFIG_ED25519_VERIFY),y)
CSRC += $(DIR)/ed25519/ed25519_test.c
endif

ifeq ($(CONFIG_THERMAL_VERIFY),y)
CSRC += $(DIR)/thermal/ameba_thermal_test.c
endif

ifeq ($(CONFIG_CHIP_RST_VERIFY),y)
CSRC += $(DIR)/chip_reset/ameba_reset_test.c
endif

ifeq ($(CONFIG_DBG_TIMER_VERIFY),y)
CSRC += $(DIR)/timer/ameba_dbg_timer_test.c
endif

ifeq ($(CONFIG_SPINAND_VERIFY),y)
CSRC += $(DIR)/spi_nand/ameba_spinand_test.c
endif

ifeq ($(CONFIG_LOGUART_VERIFY),y)
CSRC += $(DIR)/loguart/ameba_loguart_test.c
CSRC += $(DIR)/loguart/hal_loguart_test.c
endif

ifeq ($(CONFIG_WIFI_VERIFY),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/core
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/intf
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halmac
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halcommon
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halbtc
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phl
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/rtl8721da
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/rtl8721da_rom
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/wifi_fw/amebdplus/include
CSRC += $(DIR)/wifi/amebadplus_wifi_olt.c
endif

ifeq ($(CONFIG_ZB_INT_VERIFY),y)
CSRC += $(DIR)/zb_interrupt/ameba_zb_interrupt_test.c
endif

ifeq ($(CONFIG_DBGPORT_VERIFY),y)
CSRC += $(DIR)/ameba_debugport.c
endif

ifeq ($(CONFIG_MEMORY_VERIFY),y)
CSRC += $(DIR)/memory_share/ameba_memory_test.c
endif

ifeq ($(CONFIG_KEYSCAN_VERIFY),y)
CSRC += $(DIR)/keyscan/ameba_keyscan_test.c
endif

ifeq ($(CONFIG_SDM_VERIFY),y)
MODULE_IFLAGS += -I$(DIR)/sdm
CSRC += $(DIR)/sdm/ameba_sdm_test.c
endif

ifeq ($(CONFIG_QSPI_VERIFY),y)
CSRC += $(DIR)/qspi/qspi_lcd_test.c
endif

ifeq ($(CONFIG_PPE_VERIFY),y)
CSRC += $(DIR)/ppe/ameba_ppe_test.c
endif

ifeq ($(CONFIG_MBEDTLS_VERIFY),y)
CSRC += $(DIR)/mbedtls/mbedtls_test_km4.c
endif

ifeq ($(CONFIG_FLASH_AVL),y)
CSRC += $(DIR)/mbedtls/ameba_spi_flash_qc_test.c
CSRC += $(DIR)/mbedtls/flash_avl_ameba_flashclk.c
endif
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
