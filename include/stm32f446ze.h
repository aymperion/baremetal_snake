#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define __I volatile const      // Read Only Register
#define __O volatile            // Write Only Register
#define __IO volatile           // Read-Write Register
#define RESERVED volatile const // Reserved registers

#define GPIOA_BASE 0x40020000
#define GPIOB_BASE 0x40020400
#define GPIOC_BASE 0x40020800
#define GPIOD_BASE 0x40020C00
#define SPI3_BASE 0x40003C00

#define RCC_BASE 0x40023800
#define RCC_AHB1ENR_OFFSET 0x30
#define RCC_APB1ENR_OFFSET 0x40
#define RCC_APB2ENR_OFFSET 0x44
#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))
#define RCC_APB1ENR (*(volatile uint32_t *)(RCC_BASE + RCC_APB1ENR_OFFSET))
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + RCC_APB2ENR_OFFSET))

typedef struct {
  __IO uint32_t MODER;
  __IO uint16_t OTYPER;
  RESERVED uint16_t RESERVED1;
  __IO uint32_t OSPEEDR;
  __IO uint32_t PUPDR;
  __I uint16_t IDR;
  RESERVED uint16_t RESERVED2;
  __IO uint16_t ODR;
  RESERVED uint16_t RESERVED3;
  __O uint32_t BSRR;
  union {
    volatile uint32_t LCKR; // direct access to the LCKR register
    struct {
      __IO uint32_t LCK : 16;           // bits 0-15
      __IO uint32_t LCKK : 1;           // bit 16
      RESERVED uint32_t RESERVED4 : 15; // bits 17-31 (reserved)
    } LCKR_bits;
  };
  __IO uint32_t AFRL;
  __IO uint32_t AFRH;
} GPIOx_RegDef_t;

typedef struct {
  __IO uint16_t CR1;
  RESERVED uint16_t RESERVED1;

  __IO u_int16_t CR2;
  RESERVED uint16_t RESERVED2;

  __I u_int16_t SR;
  RESERVED uint16_t RESERVED3;

  __IO uint16_t DR;
  RESERVED uint16_t RESERVED4;

  __IO uint16_t CRCPR;
  RESERVED uint16_t RESERVED5;

  __I uint16_t RXCRCR;
  RESERVED uint16_t RESERVED6;

  __I uint16_t TXCRCR;
  RESERVED uint16_t RESERVED7;

  __IO uint16_t I2SCFGR;
  RESERVED uint16_t RESERVED8;

  __IO uint16_t I2SPR;
} SPIx_RegDef_t;

#define SPI3 ((SPIx_RegDef_t*)SPI3_BASE)
#define GPIOA ((GPIOx_RegDef_t *)GPIOA_BASE)
#define GPIOB ((GPIOx_RegDef_t *)GPIOB_BASE)
#define GPIOC ((GPIOx_RegDef_t *)GPIOC_BASE)
#define GPIOD ((GPIOx_RegDef_t *)GPIOD_BASE)

#define STK_BASE 0xE000E010
#define STK_CTRL_OFFSET 0x00
#define STK_LOAD_OFFSET 0x04
#define STK_VAL_OFFSET 0x08

#define STK_CTRL ((volatile uint32_t*)(STK_BASE + STK_CTRL_OFFSET))
#define STK_LOAD ((volatile uint32_t*)(STK_BASE + STK_LOAD_OFFSET))
#define STK_VAL ((volatile uint32_t*)(STK_BASE + STK_VAL_OFFSET))

#define SYSCFG_BASE 0x40013800
#define SYSCFG_EXTICR2_OFFSET 0x0C
#define SYSCFG_EXTICR4_OFFSET 0x14

#define SYSCFG_EXTICR2 ((volatile uint32_t*)(SYSCFG_BASE + SYSCFG_EXTICR2_OFFSET))
#define SYSCFG_EXTICR4 ((volatile uint32_t*)(SYSCFG_BASE + SYSCFG_EXTICR4_OFFSET))

#define NVIC_BASE 0xE000E000
#define NVIC_ISER0_OFFSET (0x100 + 0x00)
#define NVIC_ISER1_OFFSET (0x100 + 0x04)

#define NVIC_ICER0_OFFSET (0x180 + 0x00)
#define NVIC_ICER1_OFFSET (0x180 + 0x04)

#define NVIC_ISER0 ((volatile uint32_t*)(NVIC_BASE + NVIC_ISER0_OFFSET))
#define NVIC_ISER1 ((volatile uint32_t*)(NVIC_BASE + NVIC_ISER1_OFFSET))

#define NVIC_ICER0 ((volatile uint32_t*)(NVIC_BASE + NVIC_ICER0_OFFSET))
#define NVIC_ICER1 ((volatile uint32_t*)(NVIC_BASE + NVIC_ICER1_OFFSET))

typedef struct {
  __IO uint32_t IMR : 23;
  RESERVED uint32_t : 9;
  __IO uint32_t EMR : 23;
  RESERVED uint32_t : 9;
  __IO uint32_t RTSR : 23;
  RESERVED uint32_t : 9;
  __IO uint32_t FTSR : 23;
  RESERVED uint32_t : 9;
  __IO uint32_t SWIER : 23;
  RESERVED uint32_t : 9;
  __IO uint32_t PR : 23;
  RESERVED uint32_t : 9;

} EXTI_RegDef_t;

#define EXTI_BASE 0x40013C00
#define EXTI ((EXTI_RegDef_t *)EXTI_BASE)