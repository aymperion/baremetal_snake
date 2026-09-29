#include "stm32f446ze.h"
#include <stdint.h>

#ifndef _NVIC_HELPER_H_
#define _NVIC_HELPER_H_

#define __disable_irq() __asm__ volatile("cpsid i" : : : "memory")
#define __enable_irq() __asm__ volatile("cpsie i" : : : "memory")

#define EXTI0_IRQn 6
#define EXTI1_IRQn 7
#define EXTI2_IRQn 8
#define EXTI3_IRQn 9
#define EXTI4_IRQn 10
#define EXTI9_5_IRQn 23
#define EXTI15_10_IRQn 40

static inline void NVIC_EnableIRQ(uint8_t irqn) {
  // x >> 5 : integer part of x / 32
  // x & 0x1F (0b00011111, 31) : x % 32
  NVIC->ISER[irqn >> 5] = (1UL << (irqn & 0x1F));
}

static inline void NVIC_DisableIRQ(uint8_t irqn) {
  // x >> 5 : integer part of x / 32
  // x & 0x1F (0b00011111, 31) : x % 32
  NVIC->ICER[irqn >> 5] = (1UL << (irqn & 0x1F));
}

#endif // _NVIC_HELPER_H_