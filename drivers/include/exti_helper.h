/**
 * @file exti_helper.h
 * @brief Driver and helper functions for External Interrupt/Event Controller (EXTI).
 * @details Handles edge detection triggers (rising/falling), mode selection (interrupt/event),
 *          and routing through SYSCFG to NVIC vectors for STM32F446xE.
 */

#ifndef _EXTI_DRIVER_H_
#define _EXTI_DRIVER_H_

#include <stdint.h>

/**
 * @brief Trigger edge selection for an EXTI line.
 */
typedef enum {
    EXTI_TRIGGER_RISING,         /**< Trigger on rising edge */
    EXTI_TRIGGER_FALLING,        /**< Trigger on falling edge */
    EXTI_TRIGGER_RISING_FALLING  /**< Trigger on both rising and falling edges */
} EXTI_Trigger;

/**
 * @brief Operating mode for an EXTI line.
 */
typedef enum {
    EXTI_MODE_EVENT,     /**< Event mode (CPU wakeup without executing an ISR) */
    EXTI_MODE_INTERRUPT  /**< Interrupt mode (generates an interrupt request to NVIC) */
} EXTI_Mode;

/**
 * @brief Configuration structure for an EXTI line.
 */
typedef struct {
    uint8_t Port;         /**< Source GPIO port index (e.g., 0 for GPIOA, 1 for GPIOB, etc.) */
    uint8_t Line;         /**< Target EXTI line from 0 to 15 (identical to GPIO pin index) */
    EXTI_Trigger Trigger; /**< Edge trigger selection @ref EXTI_Trigger */
    EXTI_Mode Mode;       /**< Routing mode @ref EXTI_Mode */
} EXTI_Config;

/**
 * @brief Configures an EXTI line and sets the SYSCFG multiplexer.
 * @details Maps the EXTI line to the requested GPIO port (via SYSCFG->EXTICR), configures
 *          the edge detection registers (RTSR / FTSR), clears any pre-existing pending flag in PR,
 *          and enables the interrupt mask (IMR) or event mask (EMR).
 * @param[in] Config Pointer to the @ref EXTI_Config structure.
 */
void EXTI_SetConfig(EXTI_Config* Config);

/**
 * @brief Reads the state of all pending interrupt requests (EXTI_PR register).
 * @return Raw 32-bit value of the EXTI_PR register.
 */
uint32_t EXTI_GetPending(void);

/**
 * @brief Clears the pending interrupt flag for a given EXTI line.
 * @details Writes a '1' to the corresponding bit in the EXTI_PR register (rc_w1).
 * @param[in] Line EXTI line number (0 to 15) to clear.
 */
void EXTI_ClearPending(uint8_t Line);

/**
 * @brief Software-triggers an interrupt or event on an EXTI line.
 * @details Writes to the SWIER (Software Interrupt Event Register).
 * @param[in] Line EXTI line number (0 to 15) to trigger.
 */
void EXTI_TriggerSoftwareIE(uint8_t Line);

/**
 * @brief Enables in the NVIC all interrupt vectors associated with active EXTI lines in IMR.
 * @details Checks active lines in EXTI->IMR and sets the appropriate bits in ISER[0] and ISER[1].
 */
void EXTI_EnableNVIC(void);

/**
 * @brief Disables in the NVIC all interrupt vectors associated with active EXTI lines in IMR.
 * @details Writes to ICER[0] and ICER[1] for vectors corresponding to active lines in EXTI->IMR.
 */
void EXTI_DisableNVIC(void);

#endif // _EXTI_DRIVER_H_