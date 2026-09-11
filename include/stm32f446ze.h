#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef _STM32F446ZE_H_
#define _STM32F446ZE_H_

#define __I volatile const // Read Only Register
#define __O volatile       // Write Only Register
#define __IO volatile      // Read-Write Register

/*
  Core peripherals registers structs
*/

typedef struct {
  __IO uint32_t ISER[8];
  uint32_t RESERVED0[24];
  __IO uint32_t ICER[8];
  uint32_t RESERVED1[24];
  __IO uint32_t ISPR[8];
  uint32_t RESERVED2[24];
  __IO uint32_t ICPR[8];
  uint32_t RESERVED3[24];
  __IO uint32_t IABR[8];
  uint32_t RESERVED4[56];
  __IO uint32_t IPR[60];
  uint32_t RESERVED5[644];
  __O uint32_t STIR;
} NVIC_RegDef_t;

typedef struct {
  __IO uint32_t CTRL;
  __IO uint32_t LOAD;
  __IO uint32_t VAL;
  __I uint32_t CALIB;
} STK_RegDef_t;

#define STK_CTRL_ENABLE_Pos 0U
#define STK_CTRL_ENABLE_Msk (1UL << STK_CTRL_ENABLE_Pos)

#define STK_CTRL_TICKINT_Pos 1U
#define STK_CTRL_TICKINT_Msk (1UL << STK_CTRL_TICKINT_Pos)

#define STK_CTRL_CLKSOURCE_Pos 2U
#define STK_CTRL_CLKSOURCE_Msk (1UL << STK_CTRL_CLKSOURCE_Pos)

#define STK_CTRL_COUNTFLAG_Pos 16U
#define STK_CTRL_COUNTFLAG_Msk (1UL << STK_CTRL_COUNTFLAG_Pos)

/*
  Chip peripherals registers structs
*/

typedef struct {
  __IO uint32_t MODER;
  __IO uint16_t OTYPER;
  uint16_t : 16;
  __IO uint32_t OSPEEDR;
  __IO uint32_t PUPDR;
  __I uint16_t IDR;
  uint16_t : 16;
  __IO uint16_t ODR;
  uint16_t : 16;
  __O uint32_t BSRR;
  __IO uint32_t LCKR; // 32-bit access only
  __IO uint32_t AFRL;
  __IO uint32_t AFRH;
} GPIOx_RegDef_t;

typedef struct {
  __IO uint16_t CR1;
  uint16_t : 16;

  __IO u_int16_t CR2;
  uint16_t : 16;

  __I u_int16_t SR;
  uint16_t : 16;

  __IO uint16_t DR;
  uint16_t : 16;

  __IO uint16_t CRCPR;
  uint16_t  : 16;

  __I uint16_t RXCRCR;
  uint16_t : 16;

  __I uint16_t TXCRCR;
  uint16_t : 16;

  __IO uint16_t I2SCFGR;
  uint16_t : 16;

  __IO uint16_t I2SPR;
} SPIx_RegDef_t;

typedef struct {
  __IO uint32_t MEMRMP;
  __IO uint32_t PMC;
  __IO uint32_t EXTICR1;
  __IO uint32_t EXTICR2;
  __IO uint32_t EXTICR3;
  __IO uint32_t EXTICR4;
  uint32_t RESERVED1[2];
  __IO uint32_t CMPCR;
  uint32_t RESERVED2[2];
  __IO uint32_t CFGR;
} SYSCFG_RegDef_t;

typedef struct {
  __IO uint32_t IMR;
  __IO uint32_t EMR;
  __IO uint32_t RTSR;
  __IO uint32_t FTSR;
  __IO uint32_t SWIER;
  __IO uint32_t PR;

} EXTI_RegDef_t;

/*
  Core peripherals adresses
*/
#define STK_BASE  0xE000E010
#define NVIC_BASE 0xE000E100

/*
  Chip peripherals adresses
*/

#define SYSCFG_BASE 0x40013800

#define RCC_BASE 0x40023800
#define RCC_AHB1ENR_OFFSET 0x30
#define RCC_APB1ENR_OFFSET 0x40
#define RCC_APB2ENR_OFFSET 0x44

#define GPIOA_BASE 0x40020000
#define GPIOB_BASE 0x40020400
#define GPIOC_BASE 0x40020800
#define GPIOD_BASE 0x40020C00
#define GPIOE_BASE 0x40021000
#define GPIOF_BASE 0x40021400
#define GPIOG_BASE 0x40021800
#define GPIOH_BASE 0x40021C00

#define SPI3_BASE 0x40003C00

#define EXTI_BASE 0x40013C00

/*
  Core peripherals definition
*/

  #define STK ((STK_RegDef_t *)(STK_BASE))
  #define NVIC ((NVIC_RegDef_t *)NVIC_BASE)
/*
  Chip peripherals definition
*/

#define GPIOA ((GPIOx_RegDef_t *)GPIOA_BASE)
#define GPIOB ((GPIOx_RegDef_t *)GPIOB_BASE)
#define GPIOC ((GPIOx_RegDef_t *)GPIOC_BASE)
#define GPIOD ((GPIOx_RegDef_t *)GPIOD_BASE)
#define GPIOE ((GPIOx_RegDef_t *)GPIOE_BASE)
#define GPIOF ((GPIOx_RegDef_t *)GPIOF_BASE)
#define GPIOG ((GPIOx_RegDef_t *)GPIOG_BASE)
#define GPIOH ((GPIOx_RegDef_t *)GPIOH_BASE)

#define SYSCFG ((SYSCFG_RegDef_t *)SYSCFG_BASE)

#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))
#define RCC_APB1ENR (*(volatile uint32_t *)(RCC_BASE + RCC_APB1ENR_OFFSET))
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + RCC_APB2ENR_OFFSET))

#define EXTI ((EXTI_RegDef_t *)EXTI_BASE)

#define SPI3 ((SPIx_RegDef_t *)SPI3_BASE)

#endif // _STM32F446ZE_H_