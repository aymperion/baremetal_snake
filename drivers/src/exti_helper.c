#include "exti_helper.h"
#include "nvic_helper.h"
#include <stdint.h>

void EXTI_SetConfig(EXTI_Config *Config) {
  uint8_t line = Config->Line;
  uint8_t trigger = Config->Trigger;

  /* Configure SYSCFG EXTICR for line 0..15*/
  uint8_t reg_idx = line >> 2U;              // line / 4 (EXTICR[0..3])
  uint8_t bit_shift = (line & 0x03U) << 2U;  // (line % 4) * 4 bits

  SYSCFG->EXTICR[reg_idx] &= ~(0x0FU << bit_shift);
  SYSCFG->EXTICR[reg_idx] |= ((uint32_t)(Config->Port & 0x0FU) << bit_shift);

  EXTI->RTSR &= ~(0b1U << line);
  EXTI->FTSR &= ~(0b1U << line);
  if (trigger == EXTI_TRIGGER_RISING) {
    EXTI->RTSR |= (0b1U << line);
  } else if (trigger == EXTI_TRIGGER_FALLING) {
    EXTI->FTSR |= (0b1U << line);
  } else if (trigger == EXTI_TRIGGER_RISING_FALLING) {
    EXTI->RTSR |= (0b1U << line);
    EXTI->FTSR |= (0b1U << line);
  }

  EXTI->PR = (0b1U << line); // clearing any pending trigger

  if (Config->Mode == EXTI_MODE_INTERRUPT) {
    EXTI->IMR |= (0b1U << line);
    EXTI->EMR &= ~(0b1U << line);
  } else {
    EXTI->EMR |= (0b1U << line);
    EXTI->IMR &= ~(0b1U << line);
  }
}

inline uint32_t EXTI_GetPending() { return EXTI->PR; }

inline void EXTI_ClearPending(uint8_t Line) { EXTI->PR = (0b1U << Line); }

inline void EXTI_TriggerSoftwareIE(uint8_t Line) { EXTI->SWIER |= (0b1U << Line); }

void EXTI_EnableNVIC(void) {
  uint16_t mask = EXTI->IMR;
  uint32_t iser0_mask = 0;
  uint32_t iser1_mask = 0;

  /* Lines 0..4 have dedicated NVIC IRQs (EXTI0..EXTI4 -> IRQ 6..10) */
  for (uint8_t i = 0; i <= 4; i++) {
    if (mask & (1U << i)) {
      iser0_mask |= (1UL << (EXTI0_IRQn + i));
    }
  }

  /* Lines 5..9 share EXTI9_5_IRQn (IRQ 23 -> bit 23 in ISER[0]) */
  if (mask & (0x1FU << 5)) {
    iser0_mask |= (1UL << EXTI9_5_IRQn);
  }

  /* Lines 10..15 share EXTI15_10_IRQn (IRQ 40 -> bit 8 in ISER[1]) */
  if (mask & (0x3FU << 10)) {
    iser1_mask |= (1UL << (EXTI15_10_IRQn - 32));
  }

  /* Direct atomic single writes (Write-1-to-Set) */
  if (iser0_mask) {
    NVIC->ISER[0] = iser0_mask;
  }
  if (iser1_mask) {
    NVIC->ISER[1] = iser1_mask;
  }
}

void EXTI_DisableNVIC(void) {
  uint16_t mask = EXTI->IMR;
  uint32_t icer0_mask = 0;
  uint32_t icer1_mask = 0;

  for (uint8_t i = 0; i <= 4; i++) {
    if (mask & (1U << i)) {
      icer0_mask |= (1UL << (EXTI0_IRQn + i));
    }
  }

  if (mask & (0x1FU << 5)) {
    icer0_mask |= (1UL << EXTI9_5_IRQn);
  }

  if (mask & (0x3FU << 10)) {
    icer1_mask |= (1UL << (EXTI15_10_IRQn - 32));
  }

  /* Direct atomic single writes (Write-1-to-Clear) */
  if (icer0_mask) {
    NVIC->ICER[0] = icer0_mask;
  }
  if (icer1_mask) {
    NVIC->ICER[1] = icer1_mask;
  }
}