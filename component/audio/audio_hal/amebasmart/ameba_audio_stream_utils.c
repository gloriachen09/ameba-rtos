/*
 * Copyright (c) 2022 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "basic_types.h"

#include "ameba.h"
#include "ameba_audio_stream.h"
#include "ameba_audio_types.h"

#include "audio_hw_debug.h"
#include "audio_hw_osal_errnos.h"

#include "ameba_audio_stream_utils.h"

#ifdef CONFIG_ARM_CORE_CA32
#include "FreeRTOS.h"
#endif

AUD_TypeDef *g_audio_analog = AUD_SYS_BASE;

int32_t ameba_audio_get_channel(uint32_t channel_count)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel_count) {
	case 1:
		tmp = SP_CH_MONO;
		break;
	case 2:
	case 4:
	case 6:
	case 8:
		tmp = SP_CH_STEREO;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_sp_tdm(uint32_t channel_count)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel_count) {
	case 1:
	case 2:
		tmp = SP_TX_NOTDM;
		break;
	case 4:
		tmp = SP_TX_TDM4;
		break;
	case 6:
		tmp = SP_TX_TDM6;
		break;
	case 8:
		tmp = SP_TX_TDM8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_codec_tdm(uint32_t channel_count)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel_count) {
	case 1:
	case 2:
		tmp = I2S_NOTDM;
		break;
	case 4:
		tmp = I2S_TDM4;
		break;
	case 6:
		tmp = I2S_TDM6;
		break;
	case 8:
		tmp = I2S_TDM8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_fifo_num(uint32_t channel_count)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel_count) {
	case 1:
	case 2:
		tmp = SP_RX_FIFO2;
		break;
	case 4:
		tmp = SP_RX_FIFO4;
		break;
	case 6:
		tmp = SP_RX_FIFO6;
		break;
	case 8:
		tmp = SP_RX_FIFO8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_sp_format(enum AudioHwFormat format, uint32_t direction)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (format) {
	case AUDIO_HW_FORMAT_PCM_32_BIT:
		if (direction == STREAM_OUT) {
			tmp = SP_TXWL_32;
		} else {
			tmp = SP_RXWL_32;
		}
		break;
	case AUDIO_HW_FORMAT_PCM_8_24_BIT:
		if (direction == STREAM_OUT) {
			tmp = SP_TXWL_24;
		} else {
			tmp = SP_RXWL_24;
		}
		break;
	case AUDIO_HW_FORMAT_PCM_16_BIT:
		if (direction == STREAM_OUT) {
			tmp = SP_TXWL_16;
		} else {
			tmp = SP_RXWL_16;
		}
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_sp_rate(uint32_t rate)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (rate) {
	case 8000:
		tmp = SP_8K;
		break;
	case 11025:
		tmp = SP_11P025K;
		break;
	case 16000:
		tmp = SP_16K;
		break;
	case 22050:
		tmp = SP_22P05K;
		break;
	case 24000:
		tmp = SP_24K;
		break;
	case 32000:
		tmp = SP_32K;
		break;
	case 44100:
		tmp = SP_44P1K;
		break;
	case 48000:
		tmp = SP_48K;
		break;
	case 88200:
		tmp = SP_88P2K;
		break;
	case 96000:
		tmp = SP_96K;
		break;
	case 192000:
		tmp = SP_192K;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid rate");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_codec_format(enum AudioHwFormat format, uint32_t direction)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	(void)direction;
	switch (format) {
	case AUDIO_HW_FORMAT_PCM_8_24_BIT:
		tmp = WL_24;
		break;
	case AUDIO_HW_FORMAT_PCM_16_BIT:
		tmp = WL_16;
		break;
	case AUDIO_HW_FORMAT_PCM_8_BIT:
		tmp = WL_8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_codec_rate(uint32_t rate)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (rate) {
	case 8000:
		tmp = SR_8K;
		break;
	case 11025:
		tmp = SR_11P025K;
		break;
	case 16000:
		tmp = SR_16K;
		break;
	case 22050:
		tmp = SR_22P05K;
		break;
	case 24000:
		tmp = SR_24K;
		break;
	case 32000:
		tmp = SR_32K;
		break;
	case 44100:
		tmp = SR_44P1K;
		break;
	case 48000:
		tmp = SR_48K;
		break;
	case 88200:
		tmp = SR_88P2K;
		break;
	case 96000:
		tmp = SR_96K;
		break;
	case 192000:
		tmp = SR_192K;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid rate");
		break;
	}
	return tmp;
}

int32_t ameba_audio_stream_get_mic_idx(uint32_t mic_category)
{
	int32_t mic_num = 0;
	switch (mic_category) {
	case AMEBA_AUDIO_AMIC1:
		mic_num = AMIC1;
		break;
	case AMEBA_AUDIO_AMIC2:
		mic_num = AMIC2;
		break;
	case AMEBA_AUDIO_AMIC3:
		mic_num = AMIC3;
		break;
	case AMEBA_AUDIO_AMIC4:
		mic_num = AMIC4;
		break;
	case AMEBA_AUDIO_AMIC5:
		mic_num = AMIC5;
		break;
	case AMEBA_AUDIO_DMIC1:
		mic_num = DMIC1;
		break;
	case AMEBA_AUDIO_DMIC2:
		mic_num = DMIC2;
		break;
	case AMEBA_AUDIO_DMIC3:
		mic_num = DMIC3;
		break;
	case AMEBA_AUDIO_DMIC4:
		mic_num = DMIC4;
		break;
	case AMEBA_AUDIO_DMIC5:
		mic_num = DMIC5;
		break;
	case AMEBA_AUDIO_DMIC6:
		mic_num = DMIC6;
		break;
	case AMEBA_AUDIO_DMIC7:
		mic_num = DMIC7;
		break;
	case AMEBA_AUDIO_DMIC8:
		mic_num = DMIC8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] mic category %ld not supported", mic_category);
		return HAL_OSAL_ERR_INVALID_PARAM;
	}

	return mic_num;
}

int32_t ameba_audio_stream_get_adc_chn_idx(uint32_t index)
{
	int32_t adc_chn = HAL_OSAL_ERR_INVALID_PARAM;
	switch (index) {
	case 1:
		adc_chn = ADCHN1;
		break;
	case 2:
		adc_chn = ADCHN2;
		break;
	case 3:
		adc_chn = ADCHN3;
		break;
	case 4:
		adc_chn = ADCHN4;
		break;
	case 5:
		adc_chn = ADCHN5;
		break;
	case 6:
		adc_chn = ADCHN6;
		break;
	case 7:
		adc_chn = ADCHN7;
		break;
	case 8:
		adc_chn = ADCHN8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] adc channel index: %ld not supported", index);
		break;
	}

	return adc_chn;
}

int32_t ameba_audio_stream_get_adc_idx(uint32_t index)
{
	int32_t adc_num = HAL_OSAL_ERR_INVALID_PARAM;
	switch (index) {
	case 1:
		adc_num = ADC1;
		break;
	case 2:
		adc_num = ADC2;
		break;
	case 3:
		adc_num = ADC3;
		break;
	case 4:
		adc_num = ADC4;
		break;
	case 5:
		adc_num = ADC5;
		break;
	case 6:
		adc_num = ADC6;
		break;
	case 7:
		adc_num = ADC7;
		break;
	case 8:
		adc_num = ADC8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] adc index: %ld not supported", index);
		break;
	}

	return adc_num;
}

void ameba_audio_dump_gdma_regs(u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *)GDMA_BASE);

	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].SAR:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].SAR);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].DAR:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].DAR);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].CTL_LOW:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].CTL_LOW);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].CTL_HIGH:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].CTL_HIGH);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].CFG_LOW:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].CFG_LOW);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].CFG_HIGH:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].CFG_HIGH);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].ChEnReg:%lx", GDMA_ChNum, GDMA->ChEnReg);

	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].MASK_TFR:%lx", GDMA_ChNum, GDMA->MASK_TFR);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].MASK_BLOCK:%lx", GDMA_ChNum, GDMA->MASK_BLOCK);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].MASK_ERR:%lx", GDMA_ChNum, GDMA->MASK_ERR);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].STATUS_BLOCK:%lx", GDMA_ChNum, GDMA->STATUS_BLOCK);

}

void ameba_audio_dump_sport_regs(uint32_t SPORTx)
{
	int32_t tmp;
	AUDIO_SPORT_TypeDef *sportx = (AUDIO_SPORT_TypeDef *)SPORTx;
	HAL_AUDIO_DUMP_INFO("dump sportx:0x%p", sportx);
	tmp = sportx->SP_REG_MUX;
	HAL_AUDIO_DUMP_INFO("REG_SP_REG_MUX:%lx", tmp);
	tmp = sportx->SP_CTRL0;
	HAL_AUDIO_DUMP_INFO("REG_SP_CTRL0:%lx", tmp);
	tmp = sportx->SP_CTRL1;
	HAL_AUDIO_DUMP_INFO("REG_SP_CTRL1:%lx", tmp);
	tmp = sportx->SP_INT_CTRL;
	HAL_AUDIO_DUMP_INFO("REG_SP_INT_CTRL:%lx", tmp);
	tmp = sportx->RSVD0;
	HAL_AUDIO_DUMP_INFO("REG_RSVD0:%lx", tmp);
	tmp = sportx->SP_TRX_COUNTER_STATUS;
	HAL_AUDIO_DUMP_INFO("REG_SP_TRX_COUNTER_STATUS:%lx", tmp);
	tmp = sportx->SP_ERR;
	HAL_AUDIO_DUMP_INFO("REG_SP_ERR:%lx", tmp);
	tmp = sportx->SP_TX_BCLK;
	HAL_AUDIO_DUMP_INFO("REG_SP_SR_TX_BCLK:%lx", tmp);
	tmp = sportx->SP_TX_LRCLK;
	HAL_AUDIO_DUMP_INFO("REG_SP_TX_LRCLK:%lx", tmp);
	tmp = sportx->SP_FIFO_CTRL;
	HAL_AUDIO_DUMP_INFO("REG_SP_FIFO_CTRL:%lx", tmp);
	tmp = sportx->SP_FORMAT;
	HAL_AUDIO_DUMP_INFO("REG_SP_FORMAT:%lx", tmp);
	tmp = sportx->SP_RX_BCLK;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_BCLK:%lx", tmp);
	tmp = sportx->SP_RX_LRCLK;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_LRCLK:%lx", tmp);
	tmp = sportx->SP_DSP_COUNTER;
	HAL_AUDIO_DUMP_INFO("REG_SP_DSP_COUNTER:%lx", tmp);
	tmp = sportx->RSVD1;
	HAL_AUDIO_DUMP_INFO("REG_RSVD1:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL0;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL0:%lx", tmp);
	tmp = sportx->RSVD2;
	HAL_AUDIO_DUMP_INFO("REG_RSVD2:%lx", tmp);
	tmp = sportx->SP_FIFO_IRQ;
	HAL_AUDIO_DUMP_INFO("REG_SP_FIFO_IRQ:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL1;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL1:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL2;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL2:%lx", tmp);
	tmp = sportx->RSVD3;
	HAL_AUDIO_DUMP_INFO("REG_RSVD3:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL3;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL3:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL4;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL4:%lx", tmp);
	tmp = sportx->SP_RX_COUNTER1;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_COUNTER1:%lx", tmp);
	tmp = sportx->SP_RX_COUNTER2;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_COUNTER2:%lx", tmp);
	tmp = sportx->SP_TX_FIFO_0_WR_ADDR;
	HAL_AUDIO_DUMP_INFO("REG_SP_TX_FIFO_0_WR_ADDR:%lx", tmp);
	tmp = sportx->SP_RX_FIFO_0_RD_ADDR;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_FIFO_0_RD_ADDR:%lx", tmp);
	tmp = sportx->SP_TX_FIFO_1_WR_ADDR;
	HAL_AUDIO_DUMP_INFO("REG_SP_TX_FIFO_1_WR_ADDR:%lx", tmp);
	tmp = sportx->SP_RX_FIFO_1_RD_ADDR;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_FIFO_1_RD_ADDR:%lx", tmp);

}

void ameba_audio_dump_codec_regs(void)
{
	uint32_t tmp;
	AUDIO_TypeDef *audio_base;
	if (TrustZone_IsSecure()) {
		audio_base = (AUDIO_TypeDef *)AUDIO_REG_BASE_S;
	} else {
		audio_base = (AUDIO_TypeDef *)AUDIO_REG_BASE;
	}

	tmp = g_audio_analog->AUD_ADDA_CTL;
	HAL_AUDIO_DUMP_INFO("ADDA_CTL:%lx", tmp);
	tmp = g_audio_analog->AUD_HPO_CTL;
	HAL_AUDIO_DUMP_INFO("AUD_HPO_CTL:%lx", tmp);
	tmp = g_audio_analog->AUD_MICBIAS_CTL0;
	HAL_AUDIO_DUMP_INFO("MICBIAS_CTL0:%lx", tmp);
	tmp = g_audio_analog->AUD_MICBST_CTL0;
	HAL_AUDIO_DUMP_INFO("MICBST_CTL0:%lx", tmp);
	tmp = g_audio_analog->AUD_MICBST_CTL1;
	HAL_AUDIO_DUMP_INFO("MICBST_CTL1:%lx", tmp);
	tmp = g_audio_analog->RSVD0;
	HAL_AUDIO_DUMP_INFO("ANALOG_RSVD0:%lx", tmp);
	tmp = g_audio_analog->AUD_DTS_CTL;
	HAL_AUDIO_DUMP_INFO("DTS_CTL:%lx", tmp);
	tmp = g_audio_analog->AUD_MBIAS_CTL0;
	HAL_AUDIO_DUMP_INFO("MBIAS_CTL0:%lx", tmp);
	tmp = g_audio_analog->AUD_MBIAS_CTL1;
	HAL_AUDIO_DUMP_INFO("MBIAS_CTL1:%lx", tmp);
	tmp = g_audio_analog->AUD_MBIAS_CTL2;
	HAL_AUDIO_DUMP_INFO("MBIAS_CTL2:%lx", tmp);

	/***digital reg dump***/
	tmp = audio_base->CODEC_AUDIO_CONTROL_0;
	HAL_AUDIO_DUMP_INFO("CODEC_AUDIO_CONTROL_0:%lx", tmp);
	tmp = audio_base->CODEC_AUDIO_CONTROL_1;
	HAL_AUDIO_DUMP_INFO("CODEC_AUDIO_CONTROL_1:%lx", tmp);
	tmp = audio_base->CODEC_CLOCK_CONTROL_1;
	HAL_AUDIO_DUMP_INFO("CODEC_CLOCK_CONTROL_1:%lx", tmp);
	tmp = audio_base->CODEC_CLOCK_CONTROL_2;
	HAL_AUDIO_DUMP_INFO("CODEC_CLOCK_CONTROL_1:%lx", tmp);
	tmp = audio_base->CODEC_CLOCK_CONTROL_3;
	HAL_AUDIO_DUMP_INFO("CODEC_CLOCK_CONTROL_1:%lx", tmp);
	tmp = audio_base->CODEC_CLOCK_CONTROL_4;
	HAL_AUDIO_DUMP_INFO("CODEC_CLOCK_CONTROL_4:%lx", tmp);
	tmp = audio_base->CODEC_CLOCK_CONTROL_5;
	HAL_AUDIO_DUMP_INFO("CODEC_CLOCK_CONTROL_5:%lx", tmp);
	tmp = audio_base->CODEC_CLOCK_CONTROL_6;
	HAL_AUDIO_DUMP_INFO("CODEC_CLOCK_CONTROL_5:%lx", tmp);
	tmp = audio_base->CODEC_CLOCK_CONTROL_7;
	HAL_AUDIO_DUMP_INFO("CODEC_CLOCK_CONTROL_5:%lx", tmp);
	tmp = audio_base->CODEC_I2S_AD_SEL_CONTROL;
	HAL_AUDIO_DUMP_INFO("CODEC_I2S_AD_SEL_CONTROL:%lx", tmp);

	for (uint32_t i = 0; i < 2; i++) {
		tmp = audio_base->CODEC_I2S_SRC_CTRL[i].CODEC_I2S_x_CONTROL;
		HAL_AUDIO_DUMP_INFO("CODEC_I2S_SRC_CTRL_%lu:%lx", i, tmp);
		tmp = audio_base->CODEC_I2S_SRC_CTRL[i].CODEC_I2S_x_CONTROL_1;
		HAL_AUDIO_DUMP_INFO("CODEC_I2S_SRC_CTRL_%lu:%lx", i, tmp);
	}

	for (uint32_t i = 0; i < MAX_AD_NUM; i++) {
		tmp = audio_base->CODEC_ADC_CH_CTRL[i].CODEC_ADC_x_CONTROL_0;
		HAL_AUDIO_DUMP_INFO("CODEC_ADC_%lu_CONTROL_0:%lx", i, tmp);
		tmp = audio_base->CODEC_ADC_CH_CTRL[i].CODEC_ADC_x_CONTROL_1;
		HAL_AUDIO_DUMP_INFO("CODEC_ADC_%lu_CONTROL_1:%lx", i, tmp);
	}

	tmp = audio_base->CODEC_DAC_L_CONTROL_0;
	HAL_AUDIO_DUMP_INFO("CODEC_DAC_L_CONTROL_0:%lx", tmp);
	tmp = audio_base->CODEC_DAC_L_CONTROL_1;
	HAL_AUDIO_DUMP_INFO("CODEC_DAC_L_CONTROL_1:%lx", tmp);
	tmp = audio_base->CODEC_DAC_L_CONTROL_2;
	HAL_AUDIO_DUMP_INFO("CODEC_DAC_L_CONTROL_2:%lx", tmp);
	tmp = audio_base->CODEC_DAC_R_CONTROL_0;
	HAL_AUDIO_DUMP_INFO("CODEC_DAC_R_CONTROL_0:%lx", tmp);
	tmp = audio_base->CODEC_DAC_R_CONTROL_1;
	HAL_AUDIO_DUMP_INFO("CODEC_DAC_R_CONTROL_1:%lx", tmp);
	tmp = audio_base->CODEC_DAC_R_CONTROL_2;
	HAL_AUDIO_DUMP_INFO("CODEC_DAC_R_CONTROL_2:%lx", tmp);

}

int64_t ameba_audio_get_now_ns(void)
{
	return rtos_time_get_current_system_time_us() * 1000LL;
}

AUDIO_SPORT_TypeDef *ameba_audio_get_sport_addr(uint32_t index)
{
	AUDIO_SPORT_TypeDef *addr;
	switch (index) {
	case 0:
		addr = AUDIO_SPORT0_DEV;
		break;
	case 1:
		addr = AUDIO_SPORT1_DEV;
		break;
	case 2:
		addr = AUDIO_SPORT2_DEV;
		break;
	case 3:
		addr = AUDIO_SPORT3_DEV;
		break;
	default:
		HAL_AUDIO_ERROR("unsupported sport:%lu", index);
		addr = NULL;
		break;
	}
	return addr;
}

int32_t ameba_audio_get_i2s_pin_func(uint32_t index)
{
	int32_t pin_func = HAL_OSAL_ERR_INVALID_PARAM;
	switch (index) {
	case 0:
		pin_func = PINMUX_FUNCTION_I2S0;
		break;
	case 1:
		pin_func = PINMUX_FUNCTION_I2S1;
		break;
	case 2:
		pin_func = PINMUX_FUNCTION_I2S2;
		break;
	case 3:
		pin_func = PINMUX_FUNCTION_I2S3;
		break;
	default:
		HAL_AUDIO_ERROR("unsupported sport:%lu", index);
		break;
	}
	return pin_func;
}

int32_t ameba_audio_get_sport_irq(uint32_t index)
{
	int32_t irq = HAL_OSAL_ERR_INVALID_PARAM;
	switch (index) {
	case 0:
		irq = SPORT0_IRQ;
		break;
	case 1:
		irq = SPORT1_IRQ;
		break;
	case 2:
		irq = SPORT2_IRQ;
		break;
	case 3:
		irq = SPORT3_IRQ;
		break;
	default:
		HAL_AUDIO_ERROR("unsupported sport:%lu", index);
		break;
	}
	return irq;
}

int32_t ameba_audio_stream_get_direct_out_channel_idx(uint32_t channel)
{
	uint32_t direct_out_channel = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel)
	{
	case 0:
		direct_out_channel = DIRECT_OUT_CHN0;
		break;
	case 1:
		direct_out_channel = DIRECT_OUT_CHN1;
		break;
	case 2:
		direct_out_channel = DIRECT_OUT_CHN2;
		break;
	case 3:
		direct_out_channel = DIRECT_OUT_CHN3;
		break;
	case 4:
		direct_out_channel = DIRECT_OUT_CHN4;
		break;
	case 5:
		direct_out_channel = DIRECT_OUT_CHN5;
		break;
	case 6:
		direct_out_channel = DIRECT_OUT_CHN6;
		break;
	case 7:
		direct_out_channel = DIRECT_OUT_CHN7;
		break;
	default:
		HAL_AUDIO_ERROR("channel not supported for direct out:%ld", channel);
		break;
	}

	return direct_out_channel;
}

int32_t ameba_audio_stream_get_direct_in_channel_idx(uint32_t channel)
{
	uint32_t direct_in_channel = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel)
	{
	case 0:
		direct_in_channel = DIRECT_IN_CHN0;
		break;
	case 1:
		direct_in_channel = DIRECT_IN_CHN1;
		break;
	case 2:
		direct_in_channel = DIRECT_IN_CHN2;
		break;
	case 3:
		direct_in_channel = DIRECT_IN_CHN3;
		break;
	case 4:
		direct_in_channel = DIRECT_IN_CHN4;
		break;
	case 5:
		direct_in_channel = DIRECT_IN_CHN5;
		break;
	case 6:
		direct_in_channel = DIRECT_IN_CHN6;
		break;
	case 7:
		direct_in_channel = DIRECT_IN_CHN7;
		break;
	default:
		HAL_AUDIO_ERROR("channel not supported for direct in:%ld", channel);
		break;
	}

	return direct_in_channel;
}

int32_t ameba_audio_stream_get_direct_reg_idx(uint32_t channel)
{
	uint32_t direct_reg = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel)
	{
	case 0:
		direct_reg = DIRECT_REG_0;
		break;
	case 1:
		direct_reg = DIRECT_REG_1;
		break;
	case 2:
		direct_reg = DIRECT_REG_2;
		break;
	case 3:
		direct_reg = DIRECT_REG_3;
		break;
	case 4:
		direct_reg = DIRECT_REG_4;
		break;
	case 5:
		direct_reg = DIRECT_REG_5;
		break;
	case 6:
		direct_reg = DIRECT_REG_6;
		break;
	case 7:
		direct_reg = DIRECT_REG_7;
		break;
	default:
		HAL_AUDIO_ERROR("channel not supported for direct reg:%ld", channel);
		break;
	}

	return direct_reg;
}

int32_t ameba_audio_stream_get_sp_tx_channel_idx(uint32_t channel)
{
	uint32_t sp_tx_channel = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel)
	{
	case 0:
		sp_tx_channel = TXCHN0;
		break;
	case 1:
		sp_tx_channel = TXCHN1;
		break;
	case 2:
		sp_tx_channel = TXCHN2;
		break;
	case 3:
		sp_tx_channel = TXCHN3;
		break;
	case 4:
		sp_tx_channel = TXCHN4;
		break;
	case 5:
		sp_tx_channel = TXCHN5;
		break;
	case 6:
		sp_tx_channel = TXCHN6;
		break;
	case 7:
		sp_tx_channel = TXCHN7;
		break;
	default:
		HAL_AUDIO_ERROR("channel not supported for direct reg:%ld", channel);
		break;
	}

	return sp_tx_channel;
}

bool ameba_audio_sport_started(uint32_t index)
{
	AUDIO_SPORT_TypeDef *SPORTx = ameba_audio_get_sport_addr(index);

	return (((SPORTx->SP_CTRL0 & SP_BIT_TX_DISABLE) == 0)
		&& ((SPORTx->SP_CTRL0 & SP_BIT_START_TX) != 0)) ? true : false;
}