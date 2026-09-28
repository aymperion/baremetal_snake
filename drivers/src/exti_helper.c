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

  NVIC_Enable_EXTIx(line);
}

inline uint32_t EXTI_GetPending() { return EXTI->PR; }

inline void EXTI_ClearPending(uint8_t Line) { EXTI->PR = (0b1U << Line); }

inline void EXTI_TriggerSoftwareIE(uint8_t Line) { EXTI->SWIER |= (0b1U << Line); }