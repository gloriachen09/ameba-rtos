/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef AMEBA_FWK_AUDIO_CONFIGS_HARDWARE_AUDIO_AUDIO_HW_CONFIG_H
#define AMEBA_FWK_AUDIO_CONFIGS_HARDWARE_AUDIO_AUDIO_HW_CONFIG_H

/*
 *Audio amplifier pin, if your board has no audio amplifier pin , please set it as -1.
 */
#define AUDIO_HW_AMPLIFIER_PIN              -1

/*
 * See amplifier types defined in: component/audio/audio_driver/amp_manager/interfaces/amp_manager.h
 * enum AMP_TYPE. for example: dummy = 0
 */
#define AUDIO_HW_AMPLIFIER_TYPE              0

/*
 * if amplifer controlled by i2c, setup i2c sda pinmux here.
 */
#define AUDIO_HW_AMPLIFIER_I2C_SDA_PIN      -1

/*
 * if amplifer controlled by i2c, setup i2c scl pinmux here.
 */
#define AUDIO_HW_AMPLIFIER_I2C_SCL_PIN      -1

/*
 *After enable amplifer, it need some time to be steady.
 */
#define AUDIO_HW_AMPLIFIER_ENABLE_TIME       1

/*
 *on our demo board, when enable ht513 fade out, after sd set low, should wait 45ms.
 *when disable ht513 fade out, after sd set low, should wait 1ms as spec(but after testing, should be 9ms).
 *user should define this with own spec.
 */
#define AUDIO_HW_AMPLIFIER_DISABLE_TIME      9

/*
 * Set 1 to let hal control amplifier's enable and disable automatically.
 * Set 0 to let user control amplifier's enable and disable.
 */
#define AUDIO_HW_AMPLIFIER_CONTROL_ENABLE     1

/*
 *When no sound playing, should amplifier mute? Set 1 if user wants amplifier mute, set
 *0 if user wants amplifier stay unmute.
 */
#define AUDIO_HW_AMPLIFIER_MUTE_ENABLE        1

/*
 *DMIC clock and data pins.
 */
#define AUDIO_HW_DMIC_CLK_PIN                 _PB_3
#define AUDIO_HW_DMIC_DATA0_PIN               _PB_4

/*
 *After dmic clock enable, it need some time to be steady.
 *For customer's board, need to check with customer.
 */
#define AUDIO_HW_DMIC_STEADY_TIME             100

/*
 *Sport counter max irq X, when sport delivered X frames, there will be one interrupt.
 */
#define AUDIO_HW_MAX_SPORT_IRQ_X              134217727

/*
 *0: using xtal 40M.
 * if using PLL, please make sure to change component/soc/usrcfg/amebagreen2/ameba_bootcfg.c's clock
 * as 98.304->#define CLKCFG_0P9_AUDIO_USB
 *1: using PLL, mclk is mulitiplier of fs.
 *2: using PLL, mclk is fixed value.
 */
#define AUDIO_HW_OUT_SPORT_CLK_TYPE           0

/*
 *Define whether the external audio codec need MCLK from soc.
 */
#define AUDIO_I2S_OUT_NEED_MCLK_OUT           1

/*
 *If using pll, mclk is mulitiplier of fs. Set mulitiplier here.
 */
#define AUDIO_HW_OUT_MCLK_MULITIPLIER         256

/*
 *If using pll, mclk is fixed value. Set mclk value here.
 */
#define AUDIO_HW_OUT_FIXED_MCLK               20000000

/*
 *Define I2S out's data format
 *0 for I2S
 *1 for Left Justified
 *2 for SP_DF_PCM_A
 *3 for SP_DF_PCM_B
 */
#define AUDIO_I2S_OUT_DATA_FORMAT              0

/*
 *Define I2S out pins.
 */
#define AUDIO_I2S_OUT_MCLK_PIN                 _PA_9
#define AUDIO_I2S_OUT_BCLK_PIN                 _PA_10
#define AUDIO_I2S_OUT_LRCLK_PIN                _PA_14
#define AUDIO_I2S_OUT_DATA0_PIN                _PA_16
#define AUDIO_I2S_OUT_DATA1_PIN                _PA_15
#define AUDIO_I2S_OUT_DATA2_PIN                -1
#define AUDIO_I2S_OUT_DATA3_PIN                -1

#define AUDIO_I2S_OUT_MULTIIO_EN               1

/*
 *Aways output LRCLK, even if xrun happens.
 */
#define AUDIO_OUT_CONTINUOUS_LRCLK             1

/*
 *0: using xtal 40M.
 *1: using PLL, mclk is mulitiplier of fs.
 *2: using PLL, mclk is fixed value.
 *3: using PLL, ignore mclk.
 */
#define AUDIO_HW_IN_SPORT_CLK_TYPE             0

/*
 *Define whether the external audio codec need MCLK from soc.
 */
#define AUDIO_I2S_IN_NEED_MCLK_OUT             0

/*
 *If using pll, mclk is mulitiplier of fs. Set mulitiplier here.
 */
#define AUDIO_HW_IN_MCLK_MULITIPLIER           256

/*
 *If using pll, mclk is fixed value. Set mclk value here.
 */
#define AUDIO_HW_IN_FIXED_MCLK                 20000000

/*
 *Define I2S in role.
 *0: master.
 *1: slave.
 */
#define AUDIO_I2S_MASTER                       0
#define AUDIO_I2S_SLAVE                        1
#define AUDIO_I2S_IN_ROLE                      AUDIO_I2S_SLAVE

/*
 *Define I2S in pins.
 */
#define AUDIO_I2S_IN_MCLK_PIN                 -1
#define AUDIO_I2S_IN_BCLK_PIN                 _PA_9
#define AUDIO_I2S_IN_LRCLK_PIN                _PA_10
#define AUDIO_I2S_IN_DATA0_PIN                _PA_14
#define AUDIO_I2S_IN_DATA1_PIN                _PA_15
#define AUDIO_I2S_IN_DATA2_PIN                -1
#define AUDIO_I2S_IN_DATA3_PIN                -1

/*
 *If AUDIO_I2S_IN_MULTIIO_EN equals 1, using MULTI-IO.
 *If AUDIO_I2S_IN_MULTIIO_EN equals 0, using TDM.
 */
#define AUDIO_I2S_IN_MULTIIO_EN                0

/*
 *Define I2S in's data format
 *0 for I2S
 *1 for Left Justified
 *2 for SP_DF_PCM_A
 *3 for SP_DF_PCM_B
 */
#define AUDIO_I2S_IN_DATA_FORMAT               0

/*
 *Set 1 to enable debug for buffer level.
 *Set 0 to disable debug for buffer level.
 */
#define AUDIO_OUT_DEBUG_BUFFER_LEVEL           0

#endif
