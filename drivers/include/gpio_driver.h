/**
 * @file gpio_driver.h
 * @brief GPIO peripheral driver for STM32F446xE microcontroller.
 * @details Provides pin initialization, electrical characteristics configuration,
 *          and atomic read/write/toggle operations for General Purpose I/O ports.
 */

#ifndef _GPIO_DRIVER_H_
#define _GPIO_DRIVER_H_

#include "stm32f446ze.h"
#include <stdint.h>
#include <stdbool.h>

/** @brief Type representing a 16-bit binary pin mask (bits 0 to 15). */
typedef uint16_t GPIO_PinMask_t;

/**
 * @defgroup GPIO_Pins Individual GPIO pin masks
 * @{
 */
/* clang-format off */
enum {
  GPIO_PIN_0  = 0b0000000000000001U, /**< Pin 0  */
  GPIO_PIN_1  = 0b0000000000000010U, /**< Pin 1  */
  GPIO_PIN_2  = 0b0000000000000100U, /**< Pin 2  */
  GPIO_PIN_3  = 0b0000000000001000U, /**< Pin 3  */
  GPIO_PIN_4  = 0b0000000000010000U, /**< Pin 4  */
  GPIO_PIN_5  = 0b0000000000100000U, /**< Pin 5  */
  GPIO_PIN_6  = 0b0000000001000000U, /**< Pin 6  */
  GPIO_PIN_7  = 0b0000000010000000U, /**< Pin 7  */
  GPIO_PIN_8  = 0b0000000100000000U, /**< Pin 8  */
  GPIO_PIN_9  = 0b0000001000000000U, /**< Pin 9  */
  GPIO_PIN_10 = 0b0000010000000000U, /**< Pin 10 */
  GPIO_PIN_11 = 0b0000100000000000U, /**< Pin 11 */
  GPIO_PIN_12 = 0b0001000000000000U, /**< Pin 12 */
  GPIO_PIN_13 = 0b0010000000000000U, /**< Pin 13 */
  GPIO_PIN_14 = 0b0100000000000000U, /**< Pin 14 */
  GPIO_PIN_15 = 0b1000000000000000U, /**< Pin 15 */
  GPIO_PIN_ALL = 0xFFFFU             /**< All pins (0 to 15) */
};
/* clang-format on */
/** @} */

/** @brief Operating modes for a GPIO pin. */
typedef uint8_t GPIO_Mode_t;
enum {
  GPIO_MODE_INPUT,   /**< Digital input mode */
  GPIO_MODE_OUTPUT,  /**< General purpose output mode */
  GPIO_MODE_ALTFN,   /**< Alternate function mode (AF0 to AF15) */
  GPIO_MODE_ANALOG,  /**< Analog mode (ADC/DAC) */
  GPIO_MODE_ITR_RT,  /**< Input with interrupt on rising edge trigger */
  GPIO_MODE_ITR_FT,  /**< Input with interrupt on falling edge trigger */
  GPIO_MODE_ITR_RFT  /**< Input with interrupt on rising and falling edge triggers */
};

/** @brief Output stage driver type. */
typedef uint8_t GPIO_Output_Type_t;
enum { 
  GPIO_OTYPE_PUSHPULL,  /**< Push-Pull output */
  GPIO_OTYPE_OPENDRAIN  /**< Open-Drain output (requires external pull-up) */
};

/** @brief Maximum output switching frequency (Slew Rate). */
typedef uint8_t GPIO_Output_Speed_t;
enum {
  GPIO_OSPEED_LOW,     /**< Low speed (up to 2 MHz) */
  GPIO_OSPEED_MEDIUM,  /**< Medium speed (up to 25 MHz) */
  GPIO_OSPEED_FAST,    /**< Fast speed (up to 50 MHz) */
  GPIO_OSPEED_HIGH     /**< High speed (up to 100 MHz) */
};

/** @brief Internal pull-up / pull-down resistor configuration. */
typedef uint8_t GPIO_Pull_t;
enum { 
  GPIO_PULL_NONE, /**< Floating (no internal pull resistor) */
  GPIO_PULL_UP,   /**< Internal pull-up resistor enabled */
  GPIO_PULL_DOWN  /**< Internal pull-down resistor enabled */
};

/** @brief Alternate function selection index (AF0 to AF15). */
typedef uint8_t GPIO_AltFn_t;
enum {
  GPIO_ALTFN_0,  GPIO_ALTFN_1,  GPIO_ALTFN_2,  GPIO_ALTFN_3,
  GPIO_ALTFN_4,  GPIO_ALTFN_5,  GPIO_ALTFN_6,  GPIO_ALTFN_7,
  GPIO_ALTFN_8,  GPIO_ALTFN_9,  GPIO_ALTFN_10, GPIO_ALTFN_11,
  GPIO_ALTFN_12, GPIO_ALTFN_13, GPIO_ALTFN_14, GPIO_ALTFN_15
};

/**
 * @brief Configuration structure for initializing GPIO pins.
 */
typedef struct {
  GPIO_PinMask_t Pin_Mask;          /**< Bitmask of pins to configure (e.g., GPIO_PIN_0 | GPIO_PIN_1) */
  GPIO_Mode_t Mode;                 /**< Operating mode @ref GPIO_Mode_t */
  GPIO_Output_Speed_t Output_Speed; /**< Output slew rate @ref GPIO_Output_Speed_t (for output/AF modes) */
  GPIO_Output_Type_t Output_Type;   /**< Output driver type @ref GPIO_Output_Type_t (for output/AF modes) */
  GPIO_Pull_t Pull;                 /**< Pull resistor @ref GPIO_Pull_t */
  GPIO_AltFn_t Alternate_Func;      /**< Alternate function index (for AF mode) */
} GPIO_Init_t;

/**
 * @brief Initializes pins on a GPIO port according to the specified configuration.
 * @details Sets speed, output type, pull resistor, and alternate function first,
 *          then applies the pin mode (MODER) last to prevent transient glitches.
 *          If an interrupt mode is selected, also configures the corresponding EXTI line.
 * @param[in] GPIOx      Pointer to the GPIO port register structure (e.g., GPIOA, GPIOB).
 * @param[in] pGPIO_Init Pointer to the @ref GPIO_Init_t configuration structure.
 */
void GPIO_Init(GPIOx_RegDef_t *GPIOx, const GPIO_Init_t *pGPIO_Init);

/**
 * @brief Quickly configures a single pin as low-speed push-pull output without the need for external pull resistors.
 * @param[in] GPIOx Pointer to the GPIO port.
 * @param[in] pin   Pin index from 0 to 15.
 */
static inline void GPIO_Config_Output(GPIOx_RegDef_t *GPIOx, uint8_t pin) {
  if (pin > 15) {
    return;
  }
  GPIO_Init(GPIOx, &(GPIO_Init_t){.Pin_Mask = (1U << pin),
                                  .Mode = GPIO_MODE_OUTPUT,
                                  .Output_Type = GPIO_OTYPE_PUSHPULL,
                                  .Output_Speed = GPIO_OSPEED_LOW});
}

/**
 * @brief Quickly configures a single pin as input with the requested pull configuration.
 * @param[in] GPIOx Pointer to the GPIO port.
 * @param[in] pin   Pin index from 0 to 15.
 * @param[in] pull  Pull resistor setting @ref GPIO_Pull_t (NONE, UP, or DOWN).
 */
static inline void GPIO_Config_Input(GPIOx_RegDef_t *GPIOx, uint8_t pin,
                                     GPIO_Pull_t pull) {
  if (pin > 15) {
    return;
  }
  GPIO_Init(GPIOx, &(GPIO_Init_t){.Pin_Mask = (1U << pin),
                                  .Mode = GPIO_MODE_INPUT,
                                  .Pull = pull});
}

/**
 * @brief Resets all registers of a GPIO port to their default reset states.
 * @details Triggers the peripheral reset register via the RCC controller.
 * @param[in] GPIOx Pointer to the GPIO port to reset.
 */
void GPIO_DeInit(GPIOx_RegDef_t *GPIOx);

/**
 * @brief Reads the logic level present on a GPIO pin via the Input Data Register (IDR).
 * @details Reads the current state from the hardware register `GPIOx->IDR`.
 * @warning **Composite (multi-pin) masks:**
 *          This function returns a boolean evaluated as `(GPIOx->IDR & Pin_Mask) != 0`.
 *          - If a single-pin mask is passed (e.g., @ref GPIO_PIN_0), it returns `true` if high, `false` if low.
 *          - If a composite bitmask is passed (e.g., `GPIO_PIN_0 | GPIO_PIN_1`), it returns `true` if **at least one**
 *            of the masked pins is high. It does **not** test if all pins are high, nor does it return
 *            the raw multi-bit pattern. Prefer passing only single-pin masks to this function.
 *
 * @param[in] GPIOx    Pointer to the GPIO port register structure.
 * @param[in] Pin_Mask Pin mask to read (e.g., @ref GPIO_PIN_5). Recommended to be a single pin mask.
 * @return `true` if the tested pin(s) are at logic high (Vdd), `false` if at logic low (GND) or if all masked pins are low.
 */
bool GPIO_ReadPin(GPIOx_RegDef_t *GPIOx, GPIO_PinMask_t Pin_Mask);

/**
 * @brief Sets selected pins to logic high atomically.
 * @details Writes directly to the lower 16 bits (BS) of the BSRR register.
 * @param[in] GPIOx    Pointer to the GPIO port.
 * @param[in] Pin_Mask Mask of pins to drive high.
 */
void GPIO_SetPin(GPIOx_RegDef_t *GPIOx, GPIO_PinMask_t Pin_Mask);

/**
 * @brief Clears selected pins to logic low atomically.
 * @details Writes directly to the upper 16 bits (BR) of the BSRR register.
 * @param[in] GPIOx    Pointer to the GPIO port.
 * @param[in] Pin_Mask Mask of pins to drive low.
 */
void GPIO_ClearPin(GPIOx_RegDef_t *GPIOx, GPIO_PinMask_t Pin_Mask);

/**
 * @brief Atomically toggles the state of selected output pins.
 * @details Reads current output state (ODR) and prepares a combined BS/BR mask
 *          written in a single operation to the BSRR register to avoid critical sections.
 * @param[in] GPIOx    Pointer to the GPIO port.
 * @param[in] Pin_Mask Mask of pins to toggle.
 */
void GPIO_TogglePin(GPIOx_RegDef_t *GPIOx, GPIO_PinMask_t Pin_Mask);

/**
 * @brief Locks the hardware configuration of selected pins until next system reset.
 * @details Executes the official write/read lock sequence on the LCKR register.
 *          Once locked, configuration registers (MODER, OTYPER, OSPEEDR, PUPDR, AF) cannot be altered.
 * @param[in] GPIOx        Pointer to the GPIO port.
 * @param[in] Pins_to_lock Mask of pins to lock.
 */
void GPIO_LockPort(GPIOx_RegDef_t *GPIOx, GPIO_PinMask_t Pins_to_lock);

#endif // _GPIO_DRIVER_H_