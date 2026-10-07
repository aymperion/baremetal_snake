#include "unity_internals.h"
#include <stdint.h>
#include <stdio.h>

void test_unit_GPIO_Init_modes(void);
void test_unit_GPIO_Init_output_type(void);
void test_unit_GPIO_Init_speed_and_pull(void);
void test_unit_GPIO_Init_alternate_function(void);

void test_decoupled_GPIO_SetPin(void);
void test_decoupled_GPIO_ClearPin(void);
void test_decoupled_GPIO_ReadPin(void);
void test_decoupled_GPIO_TogglePin(void);
void test_decoupled_EXTI_interrupt_rising_trigger(void);
void test_decoupled_EXTI_interrupt_falling_trigger(void);
void test_decoupled_EXTI_interrupt_both_trigger(void);

extern void initialise_monitor_handles(void);

void _init(void) {}

int main(void) {
    initialise_monitor_handles();

    printf("\n\nWIRE TEST_GPIO_PIN_A AND TEST_GPIO_PIN_B\n\n");

    UnityBegin("test_gpio_loopback.c");

    RUN_TEST(test_unit_GPIO_Init_modes);
    RUN_TEST(test_unit_GPIO_Init_output_type);
    RUN_TEST(test_unit_GPIO_Init_speed_and_pull);
    RUN_TEST(test_unit_GPIO_Init_alternate_function);

    RUN_TEST(test_decoupled_GPIO_SetPin);
    RUN_TEST(test_decoupled_GPIO_ClearPin);
    RUN_TEST(test_decoupled_GPIO_ReadPin);
    RUN_TEST(test_decoupled_GPIO_TogglePin);
    RUN_TEST(test_decoupled_EXTI_interrupt_rising_trigger);
    RUN_TEST(test_decoupled_EXTI_interrupt_falling_trigger);
    RUN_TEST(test_decoupled_EXTI_interrupt_both_trigger);

    return UnityEnd();
}
