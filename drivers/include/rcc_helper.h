/**
 * @file rcc_helper.h
 * @brief Helper functions for the Reset and Clock Control (RCC) peripheral.
 * @details Provides clock gating management (enable/disable) and peripheral reset
 *          functionality for GPIO ports, SPI3, and SYSCFG on STM32F446xE.
 */

#ifndef _RCC_HELPER_H_
#define _RCC_HELPER_H_

#include "stm32f446ze.h"
#include <stdint.h>

/** @brief Bit position of SPI3 enable/reset in RCC APB1 registers. */
#define SPI3_BIT_OFFSET 15U

/** @brief Bit position of SYSCFG enable/reset in RCC APB2 registers. */
#define SYSCFG_BIT_OFFSET 14U

/**
 * @defgroup RCC_Clock_Enable Peripheral clock enable functions
 * @{
 */

/**
 * @brief Enables the AHB1 peripheral clock for a specific GPIO port.
 * @details Performs a dummy read on AHB1ENR after enabling to guarantee
 *          that the clock is active before any subsequent register access.
 * @param[in] GPIOx Pointer to the GPIO port register structure (GPIOA to GPIOH).
 */
void RCC_Enable_Clock_GPIOx(GPIOx_RegDef_t *GPIOx);

/**
 * @brief Enables the APB1 peripheral clock for SPI3.
 * @details Performs a dummy read on APB1ENR to guarantee clock stabilization.
 */
void RCC_Enable_Clock_SPI3(void);

/**
 * @brief Enables the APB2 peripheral clock for the System Configuration Controller (SYSCFG).
 * @details Performs a dummy read on APB2ENR to guarantee clock stabilization.
 */
void RCC_Enable_Clock_SYSCFG(void);
/** @} */

/**
 * @defgroup RCC_Clock_Disable Peripheral clock disable functions
 * @{
 */

/**
 * @brief Disables the AHB1 peripheral clock for a specific GPIO port.
 * @param[in] GPIOx Pointer to the GPIO port register structure (GPIOA to GPIOH).
 */
void RCC_Disable_Clock_GPIOx(GPIOx_RegDef_t *GPIOx);

/**
 * @brief Disables the APB1 peripheral clock for SPI3.
 */
void RCC_Disable_Clock_SPI3(void);

/**
 * @brief Disables the APB2 peripheral clock for SYSCFG.
 */
void RCC_Disable_Clock_SYSCFG(void);
/** @} */

/**
 * @defgroup RCC_Reset Peripheral reset functions
 * @{
 */

/**
 * @brief Performs a software reset of a specific GPIO port.
 * @details Sets and then clears the corresponding reset bit in the AHB1RSTR register.
 * @param[in] GPIOx Pointer to the GPIO port register structure (GPIOA to GPIOH).
 */
void RCC_Reset_GPIOx(GPIOx_RegDef_t *GPIOx);

/**
 * @brief Performs a software reset of SPI3.
 * @details Sets and then clears the corresponding reset bit in the APB1RSTR register.
 */
void RCC_Reset_SPI3(void);

/**
 * @brief Performs a software reset of SYSCFG.
 * @details Sets and then clears the corresponding reset bit in the APB2RSTR register.
 */
void RCC_Reset_SYSCFG(void);
/** @} */

#endif // _RCC_HELPER_H_
