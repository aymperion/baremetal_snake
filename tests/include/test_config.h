#ifndef _TEST_CONFIG_H_
#define _TEST_CONFIG_H_

#include "stm32f446ze.h"
#include "gpio_driver.h"

#define XSTR(s) STR(s)
#define STR(s) #s

/*
 * Loopback pins (A and B) configuration
 */
#define TEST_GPIO_PORT_A       GPIOB
#define TEST_GPIO_PIN_A        GPIO_PIN_11
#define TEST_GPIO_PIN_A_INDEX  11

#define TEST_GPIO_PORT_B       GPIOG
#define TEST_GPIO_PIN_B        GPIO_PIN_0
#define TEST_GPIO_PIN_B_INDEX  0

#endif // _TEST_CONFIG_H_
