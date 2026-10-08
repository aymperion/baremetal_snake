/**
 * @file nvic_helper.h
 * @brief Helper functions and macro definitions for the Nested Vectored Interrupt Controller (NVIC).
 * @details Provides global and individual interrupt line enable/disable operations
 *          for the ARM Cortex-M4 core on STM32F446xE.
 */

#ifndef _NVIC_HELPER_H_
#define _NVIC_HELPER_H_

#include "stm32f446ze.h"
#include <stdint.h>

/**
 * @brief Globally disables maskable interrupts.
 * @details Executes the CPSID I assembly instruction to mask all configurable-priority interrupts.
 */
#define __disable_irq() __asm__ volatile("cpsid i" : : : "memory")

/**
 * @brief Globally enables maskable interrupts.
 * @details Executes the CPSIE I assembly instruction to unmask interrupts.
 */
#define __enable_irq() __asm__ volatile("cpsie i" : : : "memory")

/**
 * @defgroup EXTI_IRQn_Vectors NVIC Interrupt Request numbers (IRQn) associated with EXTI lines
 * @{
 */
#define EXTI0_IRQn 6      /**< NVIC vector for EXTI line 0 */
#define EXTI1_IRQn 7      /**< NVIC vector for EXTI line 1 */
#define EXTI2_IRQn 8      /**< NVIC vector for EXTI line 2 */
#define EXTI3_IRQn 9      /**< NVIC vector for EXTI line 3 */
#define EXTI4_IRQn 10     /**< NVIC vector for EXTI line 4 */
#define EXTI9_5_IRQn 23   /**< Shared NVIC vector for EXTI lines 5 to 9 */
#define EXTI15_10_IRQn 40 /**< Shared NVIC vector for EXTI lines 10 to 15 */
/** @} */

/**
 * @brief Enables a specific interrupt line in the NVIC.
 * @details Performs an atomic write to the corresponding ISER (Interrupt Set-Enable Register)
 *          matching the given interrupt request number.
 * @param[in] irqn Interrupt request number (IRQn) to enable.
 */
static inline void NVIC_EnableIRQ(uint8_t irqn) {
  // x >> 5 : integer part of x / 32
  // x & 0x1F (0b00011111, 31) : x % 32
  NVIC->ISER[irqn >> 5] = (1UL << (irqn & 0x1F));
}

/**
 * @brief Disables a specific interrupt line in the NVIC.
 * @details Performs an atomic write to the corresponding ICER (Interrupt Clear-Enable Register)
 *          matching the given interrupt request number.
 * @param[in] irqn Interrupt request number (IRQn) to disable.
 */
static inline void NVIC_DisableIRQ(uint8_t irqn) {
  // x >> 5 : integer part of x / 32
  // x & 0x1F (0b00011111, 31) : x % 32
  NVIC->ICER[irqn >> 5] = (1UL << (irqn & 0x1F));
}

#endif // _NVIC_HELPER_H_