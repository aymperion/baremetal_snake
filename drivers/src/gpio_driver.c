#include "exti_helper.h"
#include "gpio_driver.h"
#include <stdint.h>


static inline EXTI_Line PinIndex_to_EXTILine(int pin) {
  return (EXTI_Line)pin;
}

static inline EXTI_GPIO GPIO_to_EXTI_Port(GPIOx_RegDef_t *GPIOx) {
  return (EXTI_GPIO)((((uint32_t)GPIOx) - GPIOA_BASE) >> 10U);
}

void GPIO_Init(GPIO_Init_t *pGPIO_Handle) {
  GPIOx_RegDef_t *gpio = pGPIO_Handle->GPIOx;
  GPIO_Pin_t pin_mask = pGPIO_Handle->Pin;
  GPIO_Mode_t mode = pGPIO_Handle->Mode;
  GPIO_Output_Type_t output_type = pGPIO_Handle->Output_Type;
  GPIO_Output_Speed_t output_speed = pGPIO_Handle->Output_Speed;
  GPIO_Pull_t pull = pGPIO_Handle->Pull;
  GPIO_AltFn_t alt_func = pGPIO_Handle->Alternate_Func;

  for (int pin_idx = 0; pin_idx < 16; pin_idx++) {
    if (pin_mask & (0b1U << pin_idx)) {

      gpio->MODER &= ~(0b11U << (2 * pin_idx));

      if (mode <= GPIO_MODE_ANALOG) {
        gpio->MODER |= (mode << (2 * pin_idx));

      } else {
        // input mode for interrupts
        gpio->MODER |= (GPIO_MODE_INPUT << (2 * pin_idx));

        EXTI_Config exti_config;
        exti_config.Mode = EXTI_MODE_INTERRUPT;
        exti_config.Line = PinIndex_to_EXTILine(pin_idx);
        exti_config.GPIOx = GPIO_to_EXTI_Port(gpio);

        if (mode == GPIO_MODE_ITR_RT) {
          exti_config.Trigger = EXTI_TRIGGER_RISING;
          EXTI_SetConfig(&exti_config);
        } else if (mode == GPIO_MODE_ITR_FT) {
          exti_config.Trigger = EXTI_TRIGGER_FALLING;
          EXTI_SetConfig(&exti_config);
        } else if (mode == GPIO_MODE_ITR_RFT) {
          exti_config.Trigger = EXTI_TRIGGER_RISING_FALLING;
          EXTI_SetConfig(&exti_config);
        }
      }

      gpio->OTYPER &= ~(0b1U << pin_idx);
      gpio->OTYPER |= (output_type << pin_idx);

      gpio->OSPEEDR &= ~(0b11 << (2 * pin_idx));
      gpio->OSPEEDR |= (output_speed << (2 * pin_idx));

      gpio->PUPDR &= ~(0b11 << (2 * pin_idx));
      gpio->PUPDR |= (pull << (2 * pin_idx));

      gpio->AFRL &= ~(0b1111U << (4 * pin_idx));
      gpio->AFRH &= ~(0b1111U << (4 * pin_idx));
      if (mode == GPIO_MODE_ALTFN) {
        if (pin_idx < 8) {
          gpio->AFRL |= (alt_func << (4 * pin_idx));
        } else {
          gpio->AFRH |= (alt_func << (4 * (pin_idx - 8)));
        }
      }
    }
  }
}

bool GPIO_ReadPin(GPIOx_RegDef_t *GPIOx, uint8_t Pin_Number) {
  return ((GPIOx->IDR >> Pin_Number) & 0b1U);
}

void GPIO_SetPin(GPIOx_RegDef_t *GPIOx, uint8_t Pin_Number) {
  GPIOx->BSRR = (0b1U << Pin_Number);
}

void GPIO_ClearPin(GPIOx_RegDef_t *GPIOx, uint8_t Pin_Number) {
  GPIOx->BSRR = (0b1U << (16 + Pin_Number));
}

void GPIO_TogglePin(GPIOx_RegDef_t *GPIOx, uint8_t Pin_Number) {
  GPIOx->ODR ^= (0b1U << Pin_Number);
}

void GPIO_WritePort(GPIOx_RegDef_t *GPIOx, uint16_t Value) {
  GPIOx->ODR = Value;
}

void GPIO_LockPort(GPIOx_RegDef_t *GPIOx, uint16_t Pin_States) {
  uint32_t temp = (1U << 16) | Pin_States;

  GPIOx->LCKR = temp;       // WR LCKR[16] = '1' + LCKR[15:0]
  GPIOx->LCKR = Pin_States; // WR LCKR[16] = '0' + LCKR[15:0]
  GPIOx->LCKR = temp;       // WR LCKR[16] = '1' + LCKR[15:0]

  temp = GPIOx->LCKR; // RD LCKR
}