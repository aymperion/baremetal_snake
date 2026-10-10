#ifndef _TEST_GPIO_LOOPBACK_H_
#define _TEST_GPIO_LOOPBACK_H_

#include "stm32f446ze.h"

#define TEST_GPIO_PORT_A GPIOB
#define TEST_GPIO_PIN_A GPIO_PIN_11
#define TEST_GPIO_PIN_A_INDEX 11

#define TEST_GPIO_PORT_B GPIOG
#define TEST_GPIO_PIN_B GPIO_PIN_0
#define TEST_GPIO_PIN_B_INDEX 0

void test_gpio_main(void);

#endif // _TEST_GPIO_LOOPBACK_H_
