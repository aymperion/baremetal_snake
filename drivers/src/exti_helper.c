#include "exti_helper.h"
#include "nvic_helper.h"
#include <stdint.h>

void EXTI_SetConfig(EXTI_Config *Config) {
  uint16_t line = Config->Line;
  uint8_t trigger = Config->Trigger;

  switch (line) {
  case 0:
  case 1:
  case 2:
  case 3:
    SYSCFG->EXTICR1 &= ~(0b1111U << (4 * line));
    SYSCFG->EXTICR1 |= (Config->GPIOx << (4 * line));
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    SYSCFG->EXTICR2 &= ~(0b1111U << (4 * (line - 4)));
    SYSCFG->EXTICR2 |= (Config->GPIOx << (4 * (line - 4)));
    break;
  case 8:
  case 9:
  case 10:
  case 11:
    SYSCFG->EXTICR3 &= ~(0b1111U << (4 * (line - 8)));
    SYSCFG->EXTICR3 |= (Config->GPIOx << (4 * (line - 8)));
    break;
  case 12:
  case 13:
  case 14:
  case 15:
    SYSCFG->EXTICR4 &= ~(0b1111U << (4 * (line - 12)));
    SYSCFG->EXTICR4 |= (Config->GPIOx << (4 * (line - 12)));
    break;
  }

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

inline void EXTI_ClearPending(EXTI_Line Line) { EXTI->PR = (0b1U << Line); }

inline void EXTI_TriggerSoftwareIE(EXTI_Line Line) { EXTI->SWIER |= (0b1U << Line); }