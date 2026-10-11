#include "rcc_helper.h"
#include <stdint.h>

/* ----------------- CLOCK ENABLE FUNCTIONS ----------------- */

void RCC_Enable_Clock_GPIOx(GPIOx_RegDef_t *GPIOx) {
  uint8_t port_idx = GPIO_PORT_TO_INDEX(GPIOx);
  RCC->AHB1ENR |= (1U << port_idx);
  /* Dummy read to ensure clock is enabled before peripheral access */
  __IO uint32_t temp = RCC->AHB1ENR;
  (void)temp;
}

void RCC_Enable_Clock_SPIx(const SPIx_RegDef_t *SPIx) {
  uintptr_t addr = (uintptr_t)SPIx;
  __IO uint32_t dummy;
  switch (addr) {
  case SPI1_BASE:
    RCC->APB2ENR |= (1U << SPI1_BIT_OFFSET);
    dummy = RCC->APB2ENR;
    break;
  case SPI2_BASE:
    RCC->APB1ENR |= (1U << SPI2_BIT_OFFSET);
    dummy = RCC->APB1ENR;
    break;
  case SPI3_BASE:
    RCC->APB1ENR |= (1U << SPI3_BIT_OFFSET);
    dummy = RCC->APB1ENR;
    break;
  case SPI4_BASE:
    RCC->APB2ENR |= (1U << SPI4_BIT_OFFSET);
    dummy = RCC->APB2ENR;
    break;
  default:
    return;
  }
  (void)dummy;
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

void RCC_Disable_Clock_SPIx(const SPIx_RegDef_t *SPIx) {
  uintptr_t addr = (uintptr_t)SPIx;
  switch (addr) {
  case SPI1_BASE:
    RCC->APB2ENR &= ~(1U << SPI1_BIT_OFFSET);
    break;
  case SPI2_BASE:
    RCC->APB1ENR &= ~(1U << SPI2_BIT_OFFSET);
    break;
  case SPI3_BASE:
    RCC->APB1ENR &= ~(1U << SPI3_BIT_OFFSET);
    break;
  case SPI4_BASE:
    RCC->APB2ENR &= ~(1U << SPI4_BIT_OFFSET);
    break;
  default:
    return;
  }
}

void RCC_Disable_Clock_SYSCFG(void) {
  RCC->APB2ENR &= ~(1U << SYSCFG_BIT_OFFSET);
}

/* ----------------- PERIPHERAL RESET FUNCTIONS ----------------- */

void RCC_Reset_GPIOx(GPIOx_RegDef_t *GPIOx) {
  uint8_t port_idx = GPIO_PORT_TO_INDEX(GPIOx);
  RCC->AHB1RSTR |= (1U << port_idx);
  RCC->AHB1RSTR &= ~(1U << port_idx);
}

void RCC_Reset_Clock_SPIx(const SPIx_RegDef_t *SPIx) {
  uintptr_t addr = (uintptr_t)SPIx;
  switch (addr) {
  case SPI1_BASE:
    RCC->APB2RSTR |= (1U << SPI1_BIT_OFFSET);
    RCC->APB2RSTR &= ~(1U << SPI1_BIT_OFFSET);
    break;
  case SPI2_BASE:
    RCC->APB1RSTR |= (1U << SPI2_BIT_OFFSET);
    RCC->APB1RSTR &= ~(1U << SPI2_BIT_OFFSET);
    break;
  case SPI3_BASE:
    RCC->APB1RSTR |= (1U << SPI3_BIT_OFFSET);
    RCC->APB1RSTR &= ~(1U << SPI3_BIT_OFFSET);
    break;
  case SPI4_BASE:
    RCC->APB2RSTR |= (1U << SPI4_BIT_OFFSET);
    RCC->APB2RSTR &= ~(1U << SPI4_BIT_OFFSET);
    break;
  default:
    return;
  }
}

void RCC_Reset_SYSCFG(void) {
  RCC->APB2RSTR |= (1U << SYSCFG_BIT_OFFSET);
  RCC->APB2RSTR &= ~(1U << SYSCFG_BIT_OFFSET);
}
