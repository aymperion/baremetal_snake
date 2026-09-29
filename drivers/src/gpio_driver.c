#include "exti_helper.h"
#include "gpio_driver.h"
#include "rcc_helper.h"
#include <stdint.h>

void GPIO_Init(GPIOx_RegDef_t *GPIOx, const GPIO_Init_t *pGPIO_Init) {
  GPIO_PinMask_t pin_mask = pGPIO_Init->Pin_Mask;
  GPIO_Mode_t mode = pGPIO_Init->Mode;
  GPIO_Output_Type_t output_type = pGPIO_Init->Output_Type;
  GPIO_Output_Speed_t output_speed = pGPIO_Init->Output_Speed;
  GPIO_Pull_t pull = pGPIO_Init->Pull;
  GPIO_AltFn_t alt_func = pGPIO_Init->Alternate_Func;

  uint32_t moder = GPIOx->MODER;
  uint32_t otyper = GPIOx->OTYPER;
  uint32_t ospeedr = GPIOx->OSPEEDR;
  uint32_t pupdr = GPIOx->PUPDR;
  uint32_t afrl = GPIOx->AFRL;
  uint32_t afrh = GPIOx->AFRH;

  for (int pin_idx = 0; pin_idx < 16; pin_idx++) {
    if (pin_mask & (0b1U << pin_idx)) {

      moder &= ~(0b11U << (2 * pin_idx));

      if (mode <= GPIO_MODE_ANALOG) {
        moder |= (mode << (2 * pin_idx));

      } else {
        // input mode for interrupts
        moder |= (GPIO_MODE_INPUT << (2 * pin_idx));

        EXTI_Config exti_config;
        exti_config.Mode = EXTI_MODE_INTERRUPT;
        exti_config.Line = (uint8_t)pin_idx;
        exti_config.Port = GPIO_PORT_TO_INDEX(GPIOx);

        if (mode == GPIO_MODE_ITR_RT) {
          exti_config.Trigger = EXTI_TRIGGER_RISING;
        } else if (mode == GPIO_MODE_ITR_FT) {
          exti_config.Trigger = EXTI_TRIGGER_FALLING;
        } else if (mode == GPIO_MODE_ITR_RFT) {
          exti_config.Trigger = EXTI_TRIGGER_RISING_FALLING;
        }

        EXTI_SetConfig(&exti_config);
      }

      otyper &= ~(0b1U << pin_idx);
      otyper |= (output_type << pin_idx);

      ospeedr &= ~(0b11U << (2 * pin_idx));
      ospeedr |= (output_speed << (2 * pin_idx));

      pupdr &= ~(0b11U << (2 * pin_idx));
      pupdr |= (pull << (2 * pin_idx));

      if (mode == GPIO_MODE_ALTFN) {
        if (pin_idx < 8) {
          afrl &= ~(0b1111U << (4 * pin_idx));
          afrl |= (alt_func << (4 * pin_idx));
        } else {
          uint8_t shift = 4 * (pin_idx - 8);
          afrh &= ~(0b1111U << shift);
          afrh |= (alt_func << shift);
        }
      }
    }
  }

  // Write once to hardware registers
  GPIOx->OTYPER = otyper;
  GPIOx->OSPEEDR = ospeedr;
  GPIOx->PUPDR = pupdr;
  if (mode == GPIO_MODE_ALTFN) {
    GPIOx->AFRL = afrl;
    GPIOx->AFRH = afrh;
  }
  /* Apply MODER last to prevent transient glitches: all electrical
   * characteristics (type, speed, pull, alternate function) must be set before
   * driving the pad. */
  GPIOx->MODER = moder;
}

void GPIO_DeInit(GPIOx_RegDef_t *GPIOx) { RCC_Reset_GPIOx(GPIOx); }

bool GPIO_ReadPin(GPIOx_RegDef_t *GPIOx, uint8_t Pin_Number) {
  if (Pin_Number > 15) {
    return false;
  }
  return ((GPIOx->IDR >> Pin_Number) & 0b1U);
}

void GPIO_SetPin(GPIOx_RegDef_t *GPIOx, GPIO_PinMask_t Pin_Mask) {
  GPIOx->BSRR = Pin_Mask;
}

void GPIO_ClearPin(GPIOx_RegDef_t *GPIOx, GPIO_PinMask_t Pin_Mask) {
  GPIOx->BSRR = (uint32_t)(Pin_Mask << 16);
}

void GPIO_TogglePin(GPIOx_RegDef_t *GPIOx, uint8_t Pin_Number) {
  if (Pin_Number > 15) {
    return;
  }
  uint32_t odr = GPIOx->ODR;
  if (odr & (1U << Pin_Number)) {
    GPIOx->BSRR = (1U << (16 + Pin_Number));
  } else {
    GPIOx->BSRR = (1U << Pin_Number);
  }
}

void GPIO_LockPort(GPIOx_RegDef_t *GPIOx, GPIO_PinMask_t Pins_to_lock) {
  uint32_t temp = (1U << 16) | Pins_to_lock;

  GPIOx->LCKR = temp;         // WR LCKR[16] = '1' + LCKR[15:0]
  GPIOx->LCKR = Pins_to_lock; // WR LCKR[16] = '0' + LCKR[15:0]
  GPIOx->LCKR = temp;         // WR LCKR[16] = '1' + LCKR[15:0]

  temp = GPIOx->LCKR; // RD LCKR
}