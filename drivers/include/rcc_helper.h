#include "stm32f446ze.h"
#include <stdint.h>

#ifndef _RCC_HELPER_H_
#define _RCC_HELPER_H_

#define SPI3_BIT_OFFSET 15U
#define SYSCFG_BIT_OFFSET 14U

/* ----------------- CLOCK ENABLE FUNCTIONS ----------------- */
void RCC_Enable_Clock_GPIOx(GPIOx_RegDef_t *GPIOx);
void RCC_Enable_Clock_SPI3(void);
void RCC_Enable_Clock_SYSCFG(void);

/* ----------------- CLOCK DISABLE FUNCTIONS ----------------- */
void RCC_Disable_Clock_GPIOx(GPIOx_RegDef_t *GPIOx);
void RCC_Disable_Clock_SPI3(void);
void RCC_Disable_Clock_SYSCFG(void);

/* ----------------- PERIPHERAL RESET FUNCTIONS ----------------- */
void RCC_Reset_GPIOx(GPIOx_RegDef_t *GPIOx);
void RCC_Reset_SPI3(void);
void RCC_Reset_SYSCFG(void);

#endif // _RCC_HELPER_H_
