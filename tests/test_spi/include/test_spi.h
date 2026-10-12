#ifndef _TEST_SPI_H_
#define _TEST_SPI_H_

#include "stm32f446ze.h"

#define TEST_SPI SPI4

#define SPI_MOSI_PORT GPIOE
#define SPI_MOSI_PIN GPIO_PIN_6
#define SPI_MOSI_PIN_ALTFN GPIO_ALTFN_5

#define SPI_MISO_PORT GPIOE
#define SPI_MISO_PIN GPIO_PIN_5
#define SPI_MISO_PIN_ALTFN GPIO_ALTFN_5

#define SPI_SCK_PORT GPIOE
#define SPI_SCK_PIN GPIO_PIN_2
#define SPI_SCK_PIN_ALTFN GPIO_ALTFN_5

void test_SPI_main(void);

#endif // _TEST_SPI_H_
