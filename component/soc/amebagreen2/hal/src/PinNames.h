
#ifndef _PINNAMES_H_
#define _PINNAMES_H_

#include "cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIN_DATA(PUPD, FUNC)	(((PUPD) << 6) | (FUNC))
#define PIN_PIN_PUPD(X)			((X) & 0x03) /* PullNone/PullUp/PullDown */
#define PIN_FUNC(X)				((X) & 0x7F) /* PINMUX_FUNCTION_XXXX */

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_GPIO
 *  @brief    MBED_GPIO driver modules.
 *  @{
 */

/** @addtogroup MBED_GPIO_Exported_Types MBED_GPIO Exported Types
  * @{
  */

/** @defgroup MBED_GPIO_Enumeration_Type Enumeration Type
  * @{
  */

/**
  * @brief enum GPIO_PORT
  */
typedef enum {
	PORT_A = 0,
	PORT_B = 1,
} GPIO_PORT;

/**
  * @brief enum PinDirection
  */
typedef enum {
	PIN_INPUT = 0,
	PIN_OUTPUT
} PinDirection;


/**
  * @brief enum PinName
  * (((port)<<5)|(pin))
  */
typedef enum {
	PA_0  = (PORT_A << 5 | 0),
	PA_1  = (PORT_A << 5 | 1),
	PA_2  = (PORT_A << 5 | 2),
	PA_3  = (PORT_A << 5 | 3),
	PA_4  = (PORT_A << 5 | 4),
	PA_5  = (PORT_A << 5 | 5),
	PA_6  = (PORT_A << 5 | 6),
	PA_7  = (PORT_A << 5 | 7),
	PA_8  = (PORT_A << 5 | 8),
	PA_9  = (PORT_A << 5 | 9),
	PA_10  = (PORT_A << 5 | 10),
	PA_11  = (PORT_A << 5 | 11),
	PA_12  = (PORT_A << 5 | 12),
	PA_13  = (PORT_A << 5 | 13),
	PA_14  = (PORT_A << 5 | 14),
	PA_15  = (PORT_A << 5 | 15),
	PA_16  = (PORT_A << 5 | 16),
	PA_17  = (PORT_A << 5 | 17),
	PA_18  = (PORT_A << 5 | 18),
	PA_19  = (PORT_A << 5 | 19),
	PA_20  = (PORT_A << 5 | 20),
	PA_21  = (PORT_A << 5 | 21),
	PA_22  = (PORT_A << 5 | 22),
	PA_23  = (PORT_A << 5 | 23),
	PA_24  = (PORT_A << 5 | 24),
	PA_25  = (PORT_A << 5 | 25),
	PA_26  = (PORT_A << 5 | 26),
	PA_27  = (PORT_A << 5 | 27),
	PA_28  = (PORT_A << 5 | 28),
	PA_29  = (PORT_A << 5 | 29),
	PA_30  = (PORT_A << 5 | 30),
	PA_31  = (PORT_A << 5 | 31),

	PB_0  = (PORT_B << 5 | 0),
	PB_1  = (PORT_B << 5 | 1),
	PB_2  = (PORT_B << 5 | 2),
	PB_3  = (PORT_B << 5 | 3),
	PB_4  = (PORT_B << 5 | 4),
	PB_5  = (PORT_B << 5 | 5),
	PB_6  = (PORT_B << 5 | 6),
	PB_7  = (PORT_B << 5 | 7),
	PB_8  = (PORT_B << 5 | 8),
	PB_9  = (PORT_B << 5 | 9),
	PB_10  = (PORT_B << 5 | 10),
	PB_11  = (PORT_B << 5 | 11),
	PB_12  = (PORT_B << 5 | 12),
	PB_13  = (PORT_B << 5 | 13),
	PB_14  = (PORT_B << 5 | 14),
	PB_15  = (PORT_B << 5 | 15),
	PB_16  = (PORT_B << 5 | 16),
	PB_17  = (PORT_B << 5 | 17),
	PB_18  = (PORT_B << 5 | 18),
	PB_19  = (PORT_B << 5 | 19),
	PB_20  = (PORT_B << 5 | 20),
	PB_21  = (PORT_B << 5 | 21),
	PB_22  = (PORT_B << 5 | 22),
	PB_23  = (PORT_B << 5 | 23),
	PB_24  = (PORT_B << 5 | 24),
	PB_25  = (PORT_B << 5 | 25),
	PB_26  = (PORT_B << 5 | 26),
	PB_27  = (PORT_B << 5 | 27),
	PB_28  = (PORT_B << 5 | 28),
	PB_29  = (PORT_B << 5 | 29),
	PB_30  = (PORT_B << 5 | 30),
	PB_31  = (PORT_B << 5 | 31),

	AD_0 = PA_20, //CH0
	AD_1 = PA_19, //CH1
	AD_2 = PA_18, //CH2
	AD_3 = PA_17, //CH3
	AD_4 = PA_15, //CH4
	AD_5 = PA_14, //CH5
	AD_6 = PA_13, //CH6
	AD_7 = PA_12, //CH7

	// Not connected
	NC = (uint32_t)0xFFFFFFFF
} PinName;

/**
  * @brief enum PinMode
  */
typedef enum {
	PullNone  = 0, //IN HIGHZ
	PullUp    = 1,
	PullDown  = 2,
	PullNoneSlp = 3,
	PullUpSlp = 4,
	PullDownSlp = 5,
	PullDefault = PullNone
} PinMode;
/** @} */

/** @} */
/** @} */
/** @} */


#ifdef __cplusplus
}
#endif

#endif
