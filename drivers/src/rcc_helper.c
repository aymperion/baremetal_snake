#include "rcc_helper.h"

/* ----------------- CLOCK ENABLE FUNCTIONS ----------------- */

void RCC_Enable_Clock_GPIOx(GPIOx_RegDef_t *GPIOx) {
  uint8_t port_idx = GPIO_PORT_TO_INDEX(GPIOx);
  RCC->AHB1ENR |= (1U << port_idx);
  /* Dummy read to ensure clock is enabled before peripheral access */
  __IO uint32_t temp = RCC->AHB1ENR;
  (void)temp;
}

void RCC_Enable_Clock_SPI3(void) {
  RCC->APB1ENR |= (1U << SPI3_BIT_OFFSET);
  __IO uint32_t temp = RCC->APB1ENR;
  (void)temp;
}

void RCC_Enable_Clock_SYSCFG(void) {
  RCC->APB2ENR |= (1U << SYSCFG_BIT_OFFSET);
  __IO uint32_t temp = RCC->APB2ENR;
  (void)temp;
}

/* ----------------- CLOCK DISABLE FUNCTIONS ----------------- */

void RCC_Disable_Clock_GPIOx(GPIOx_RegDef_t *GPIOx) {
  uint8_t port_idx = GPIO_PORT_TO_INDEX(GPIOx);
  RCC->AHB1ENR &= ~(1U << port_idx);
}

void RCC_Disable_Clock_SPI3(void) { RCC->APB1ENR &= ~(1U << SPI3_BIT_OFFSET); }

void RCC_Disable_Clock_SYSCFG(void) {
  RCC->APB2ENR &= ~(1U << SYSCFG_BIT_OFFSET);
}

/* ----------------- PERIPHERAL RESET FUNCTIONS ----------------- */

void RCC_Reset_GPIOx(GPIOx_RegDef_t *GPIOx) {
  uint8_t port_idx = GPIO_PORT_TO_INDEX(GPIOx);
  RCC->AHB1RSTR |= (1U << port_idx);
  RCC->AHB1RSTR &= ~(1U << port_idx);
}

void RCC_Reset_SPI3(void) {
  RCC->APB1RSTR |= (1U << SPI3_BIT_OFFSET);
  RCC->APB1RSTR &= ~(1U << SPI3_BIT_OFFSET);
}

void RCC_Reset_SYSCFG(void) {
  RCC->APB2RSTR |= (1U << SYSCFG_BIT_OFFSET);
  RCC->APB2RSTR &= ~(1U << SYSCFG_BIT_OFFSET);
}
