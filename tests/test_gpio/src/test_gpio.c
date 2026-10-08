#include "test_gpio.h"
#include "exti_helper.h"
#include "gpio_driver.h"
#include "nvic_helper.h"
#include "rcc_helper.h"
#include "stm32f446ze.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdbool.h>
#include <stdint.h>

static volatile uint32_t exti_trigger_count = 0;

static inline void loopback_delay(void) {
  for (volatile int i = 0; i < 50; i++) {
    __asm__ volatile("nop");
  }
}

static void handle_exti_irq(uint8_t line) {
  if (EXTI_GetPending() & (1U << line)) {
    exti_trigger_count++;
    EXTI_ClearPending(line);
  }
}

void EXTI9_5_IRQHandler(void) { handle_exti_irq(TEST_GPIO_PIN_B_INDEX); }

void EXTI15_10_IRQHandler(void) { handle_exti_irq(TEST_GPIO_PIN_B_INDEX); }

void EXTI0_IRQHandler(void) { handle_exti_irq(0); }
void EXTI1_IRQHandler(void) { handle_exti_irq(1); }
void EXTI2_IRQHandler(void) { handle_exti_irq(2); }
void EXTI3_IRQHandler(void) { handle_exti_irq(3); }
void EXTI4_IRQHandler(void) { handle_exti_irq(4); }

void setUp(void) {
  exti_trigger_count = 0;

  RCC_Enable_Clock_GPIOx(TEST_GPIO_PORT_A);
  if (TEST_GPIO_PORT_A != TEST_GPIO_PORT_B) {
    RCC_Enable_Clock_GPIOx(TEST_GPIO_PORT_B);
  }
  RCC_Enable_Clock_SYSCFG();

  /* Clear the registers matching the tested pins */
  TEST_GPIO_PORT_A->MODER &= ~(0x03U << (2 * TEST_GPIO_PIN_A_INDEX));
  TEST_GPIO_PORT_B->MODER &= ~(0x03U << (2 * TEST_GPIO_PIN_B_INDEX));
  TEST_GPIO_PORT_A->PUPDR &= ~(0x03U << (2 * TEST_GPIO_PIN_A_INDEX));
  TEST_GPIO_PORT_B->PUPDR &= ~(0x03U << (2 * TEST_GPIO_PIN_B_INDEX));
  loopback_delay();
}

void tearDown(void) {
  EXTI_DisableNVIC();
  EXTI_ClearPending(TEST_GPIO_PIN_B_INDEX);

  /* Clear the registers matching the tested pins */
  TEST_GPIO_PORT_A->MODER &= ~(0x03U << (2 * TEST_GPIO_PIN_A_INDEX));
  TEST_GPIO_PORT_B->MODER &= ~(0x03U << (2 * TEST_GPIO_PIN_B_INDEX));
  TEST_GPIO_PORT_A->PUPDR &= ~(0x03U << (2 * TEST_GPIO_PIN_A_INDEX));
  TEST_GPIO_PORT_B->PUPDR &= ~(0x03U << (2 * TEST_GPIO_PIN_B_INDEX));
}

/* UNIT TESTS : Checking if GPIO configuration registers are properly set up */

void test_unit_GPIO_Init_modes(void) {
  /* Output mode */
  GPIO_Init(TEST_GPIO_PORT_A, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_A,
                                             .Mode = GPIO_MODE_OUTPUT});
  uint32_t moder =
      (TEST_GPIO_PORT_A->MODER >> (2 * TEST_GPIO_PIN_A_INDEX)) & 0x03U;
  TEST_ASSERT_EQUAL_HEX32(0x01, moder);

  /* Input mode */
  GPIO_Init(TEST_GPIO_PORT_A, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_A,
                                             .Mode = GPIO_MODE_INPUT});
  moder = (TEST_GPIO_PORT_A->MODER >> (2 * TEST_GPIO_PIN_A_INDEX)) & 0x03U;
  TEST_ASSERT_EQUAL_HEX32(0x00, moder);

  /* Analog mode */
  GPIO_Init(TEST_GPIO_PORT_A, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_A,
                                             .Mode = GPIO_MODE_ANALOG});
  moder = (TEST_GPIO_PORT_A->MODER >> (2 * TEST_GPIO_PIN_A_INDEX)) & 0x03U;
  TEST_ASSERT_EQUAL_HEX32(0x03, moder);
}

void test_unit_GPIO_Init_output_type(void) {
  /* Open-Drain */
  GPIO_Init(TEST_GPIO_PORT_A,
            &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_A,
                           .Mode = GPIO_MODE_OUTPUT,
                           .Output_Type = GPIO_OTYPE_OPENDRAIN});
  TEST_ASSERT_TRUE((TEST_GPIO_PORT_A->OTYPER & TEST_GPIO_PIN_A) != 0);

  /* Push-Pull */
  GPIO_Init(TEST_GPIO_PORT_A,
            &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_A,
                           .Mode = GPIO_MODE_OUTPUT,
                           .Output_Type = GPIO_OTYPE_PUSHPULL});
  TEST_ASSERT_FALSE((TEST_GPIO_PORT_A->OTYPER & TEST_GPIO_PIN_A) != 0);
}

void test_unit_GPIO_Init_speed_and_pull(void) {
  /* Speed FAST (0b10) et Pull-Up (0b01) */
  GPIO_Init(TEST_GPIO_PORT_A, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_A,
                                             .Mode = GPIO_MODE_INPUT,
                                             .Output_Speed = GPIO_OSPEED_FAST,
                                             .Pull = GPIO_PULL_UP});
  uint32_t speed =
      (TEST_GPIO_PORT_A->OSPEEDR >> (2 * TEST_GPIO_PIN_A_INDEX)) & 0x03U;
  uint32_t pull =
      (TEST_GPIO_PORT_A->PUPDR >> (2 * TEST_GPIO_PIN_A_INDEX)) & 0x03U;
  TEST_ASSERT_EQUAL_HEX32(0x02, speed);
  TEST_ASSERT_EQUAL_HEX32(0x01, pull);

  /* Pull-Down (0b10) */
  GPIO_Init(TEST_GPIO_PORT_A, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_A,
                                             .Mode = GPIO_MODE_INPUT,
                                             .Pull = GPIO_PULL_DOWN});
  pull = (TEST_GPIO_PORT_A->PUPDR >> (2 * TEST_GPIO_PIN_A_INDEX)) & 0x03U;
  TEST_ASSERT_EQUAL_HEX32(0x02, pull);
}

void test_unit_GPIO_Init_alternate_function(void) {
  GPIO_Init(TEST_GPIO_PORT_A, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_A,
                                             .Mode = GPIO_MODE_ALTFN,
                                             .Alternate_Func = GPIO_ALTFN_7});

#if (TEST_GPIO_PIN_A_INDEX < 8)
  uint32_t af_val =
      (TEST_GPIO_PORT_A->AFRL >> (4 * TEST_GPIO_PIN_A_INDEX)) & 0x0FU;
#else
  uint32_t af_val =
      (TEST_GPIO_PORT_A->AFRH >> (4 * (TEST_GPIO_PIN_A_INDEX - 8))) & 0x0FU;
#endif
  TEST_ASSERT_EQUAL_HEX32(0x07, af_val);
}

/* LOOPBACK UNIT TESTS : Testing each READING (resp. WRITING) gpio function by
 * writing (resp. reading) in memory using registers */

void test_decoupled_GPIO_SetPin(void) {
  TEST_GPIO_PORT_A->MODER |= (0x01U << (2 * TEST_GPIO_PIN_A_INDEX));  // Output
  TEST_GPIO_PORT_B->MODER &= ~(0x03U << (2 * TEST_GPIO_PIN_B_INDEX)); // Input
  TEST_GPIO_PORT_A->BSRR =
      ((uint32_t)TEST_GPIO_PIN_A << 16); // Clear tested pin
  loopback_delay();

  GPIO_SetPin(TEST_GPIO_PORT_A, TEST_GPIO_PIN_A);
  loopback_delay();

  /* Checking ODR register of TEST_GPIO_PIN_A */
  TEST_ASSERT_TRUE((TEST_GPIO_PORT_A->ODR & TEST_GPIO_PIN_A) != 0);

  /* Checking IDR register of TEST_GPIO_PIN_B */
  uint16_t raw_idr = TEST_GPIO_PORT_B->IDR;
  TEST_ASSERT_TRUE((raw_idr & TEST_GPIO_PIN_B) != 0);
}

void test_decoupled_GPIO_ClearPin(void) {
  TEST_GPIO_PORT_A->MODER |= (0x01U << (2 * TEST_GPIO_PIN_A_INDEX));  // Output
  TEST_GPIO_PORT_B->MODER &= ~(0x03U << (2 * TEST_GPIO_PIN_B_INDEX)); // Input
  TEST_GPIO_PORT_A->BSRR = TEST_GPIO_PIN_A; // Set tested pin
  loopback_delay();

  GPIO_ClearPin(TEST_GPIO_PORT_A, TEST_GPIO_PIN_A);
  loopback_delay();

  /* Checking ODR register of TEST_GPIO_PIN_A */
  TEST_ASSERT_FALSE((TEST_GPIO_PORT_A->ODR & TEST_GPIO_PIN_A) != 0);

  /* Checking IDR register of TEST_GPIO_PIN_B */
  uint16_t raw_idr = TEST_GPIO_PORT_B->IDR;
  TEST_ASSERT_FALSE((raw_idr & TEST_GPIO_PIN_B) != 0);
}

void test_decoupled_GPIO_ReadPin(void) {
  TEST_GPIO_PORT_A->MODER |= (0x01U << (2 * TEST_GPIO_PIN_A_INDEX));  // Output
  TEST_GPIO_PORT_B->MODER &= ~(0x03U << (2 * TEST_GPIO_PIN_B_INDEX)); // Input

  // Set tested pin
  TEST_GPIO_PORT_A->BSRR = TEST_GPIO_PIN_A;
  loopback_delay();

  /* Test GPIO_ReadPin to read HIGH */
  bool read_high = GPIO_ReadPin(TEST_GPIO_PORT_B, TEST_GPIO_PIN_B);
  TEST_ASSERT_TRUE(read_high);

  // Clear tested pin
  TEST_GPIO_PORT_A->BSRR = ((uint32_t)TEST_GPIO_PIN_A << 16);
  loopback_delay();

  /* Test GPIO_ReadPin to read LOW */
  bool read_low = GPIO_ReadPin(TEST_GPIO_PORT_B, TEST_GPIO_PIN_B);
  TEST_ASSERT_FALSE(read_low);
}

void test_decoupled_GPIO_TogglePin(void) {
  TEST_GPIO_PORT_A->MODER |= (0x01U << (2 * TEST_GPIO_PIN_A_INDEX));  // Output
  TEST_GPIO_PORT_B->MODER &= ~(0x03U << (2 * TEST_GPIO_PIN_B_INDEX)); // Input

  TEST_GPIO_PORT_A->BSRR =
      ((uint32_t)TEST_GPIO_PIN_A << 16); // Clear tested pin
  loopback_delay();
  TEST_ASSERT_FALSE((TEST_GPIO_PORT_B->IDR & TEST_GPIO_PIN_B) != 0);

  /* Toggle : 0 -> 1 */
  GPIO_TogglePin(TEST_GPIO_PORT_A, TEST_GPIO_PIN_A);
  loopback_delay();
  TEST_ASSERT_TRUE((TEST_GPIO_PORT_B->IDR & TEST_GPIO_PIN_B) != 0);

  /* Toggle : 1 -> 0 */
  GPIO_TogglePin(TEST_GPIO_PORT_A, TEST_GPIO_PIN_A);
  loopback_delay();
  TEST_ASSERT_FALSE((TEST_GPIO_PORT_B->IDR & TEST_GPIO_PIN_B) != 0);
}

void test_decoupled_EXTI_interrupt_rising_trigger(void) {
  TEST_GPIO_PORT_A->MODER |= (0x01U << (2 * TEST_GPIO_PIN_A_INDEX)); // Output
  TEST_GPIO_PORT_A->BSRR =
      ((uint32_t)TEST_GPIO_PIN_A << 16); // Clear TEST_GPIO_PIN_A
  loopback_delay();

  /* Setting up Rising Trigger interrupt on TEST_GPIO_PORT_B */
  GPIO_Init(TEST_GPIO_PORT_B, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_B,
                                             .Mode = GPIO_MODE_ITR_RT,
                                             .Pull = GPIO_PULL_DOWN});

  EXTI_ClearPending(TEST_GPIO_PIN_B_INDEX);
  EXTI_EnableNVIC();
  __enable_irq();

  exti_trigger_count = 0;

  /* Trigger rising edge using BSRR */
  TEST_GPIO_PORT_A->BSRR = TEST_GPIO_PIN_A;
  loopback_delay();

  /* Checking if trigger was detected */
  TEST_ASSERT_EQUAL_INT(1, exti_trigger_count);

  /* Checking if EXTI->PR register was cleared */
  TEST_ASSERT_FALSE((EXTI_GetPending() & (1U << TEST_GPIO_PIN_B_INDEX)) != 0);
}

void test_decoupled_EXTI_interrupt_falling_trigger(void) {
  TEST_GPIO_PORT_A->MODER |= (0x01U << (2 * TEST_GPIO_PIN_A_INDEX)); // Output
  TEST_GPIO_PORT_A->BSRR = TEST_GPIO_PIN_A; // Set TEST_GPIO_PIN_A
  loopback_delay();

  /* Setting up Falling Trigger interrupt on TEST_GPIO_PORT_B */
  GPIO_Init(TEST_GPIO_PORT_B, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_B,
                                             .Mode = GPIO_MODE_ITR_FT,
                                             .Pull = GPIO_PULL_DOWN});

  EXTI_ClearPending(TEST_GPIO_PIN_B_INDEX);
  EXTI_EnableNVIC();
  __enable_irq();

  exti_trigger_count = 0;

  /* Trigger falling edge using BSRR */
  TEST_GPIO_PORT_A->BSRR = ((uint32_t)TEST_GPIO_PIN_A << 16);
  loopback_delay();

  /* Checking if trigger was detected */
  TEST_ASSERT_EQUAL_INT(1, exti_trigger_count);

  /* Checking if EXTI->PR register was cleared */
  TEST_ASSERT_FALSE((EXTI_GetPending() & (1U << TEST_GPIO_PIN_B_INDEX)) != 0);
}

void test_decoupled_EXTI_interrupt_both_trigger(void) {
  TEST_GPIO_PORT_A->MODER |= (0x01U << (2 * TEST_GPIO_PIN_A_INDEX)); // Output
  TEST_GPIO_PORT_A->BSRR =
      ((uint32_t)TEST_GPIO_PIN_A << 16); // Clear TEST_GPIO_PIN_A
  loopback_delay();

  /* Setting up Falling AND Rising Triggers interrupt on TEST_GPIO_PORT_B */
  GPIO_Init(TEST_GPIO_PORT_B, &(GPIO_Init_t){.Pin_Mask = TEST_GPIO_PIN_B,
                                             .Mode = GPIO_MODE_ITR_RFT,
                                             .Pull = GPIO_PULL_DOWN});

  EXTI_ClearPending(TEST_GPIO_PIN_B_INDEX);
  EXTI_EnableNVIC();
  __enable_irq();

  exti_trigger_count = 0;

  /* Trigger rising edge using BSRR */
  TEST_GPIO_PORT_A->BSRR = TEST_GPIO_PIN_A;
  loopback_delay();

  /* Checking if trigger was detected */
  TEST_ASSERT_EQUAL_INT(1, exti_trigger_count);

  /* Checking if EXTI->PR register was cleared */
  TEST_ASSERT_FALSE((EXTI_GetPending() & (1U << TEST_GPIO_PIN_B_INDEX)) != 0);

  /* Trigger falling edge using BSRR */
  TEST_GPIO_PORT_A->BSRR = ((uint32_t)TEST_GPIO_PIN_A << 16);
  loopback_delay();

  /* Checking if trigger was detected */
  TEST_ASSERT_EQUAL_INT(2, exti_trigger_count);

  /* Checking if EXTI->PR register was cleared */
  TEST_ASSERT_FALSE((EXTI_GetPending() & (1U << TEST_GPIO_PIN_B_INDEX)) != 0);
}

void test_decoupled_GPIO_LockPort() {

  RCC_Enable_Clock_GPIOx(TEST_GPIO_PORT_B);
  GPIO_Init(TEST_GPIO_PORT_B, &(GPIO_Init_t){
    .Pin_Mask = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_15,
    .Mode = GPIO_MODE_INPUT,
    .Pull = GPIO_PULL_DOWN
  });

  GPIO_LockPort(TEST_GPIO_PORT_B, GPIO_PIN_0 | GPIO_PIN_15);

  // Trying to modify the GPIO Mode of a locked pin should have no effect
  TEST_GPIO_PORT_B->MODER |= (1U << (2 * 0));
  TEST_ASSERT_EQUAL_HEX32(0U, TEST_GPIO_PORT_B->MODER & (0b11U << (2 * 0)));

  // Trying to modify the GPIO Mode of a locked pin should have no effect
  TEST_GPIO_PORT_B->MODER |= (1U << (2 * 15));
  TEST_ASSERT_EQUAL_HEX32(0U, TEST_GPIO_PORT_B->MODER & (0b11U << (2 * 15)));

  // Trying to modify the GPIO Mode of an unlocked pin should work
  TEST_GPIO_PORT_B->MODER |= (1U << (2 * 7));
  TEST_ASSERT_EQUAL_HEX32(1U << (2 * 7), TEST_GPIO_PORT_B->MODER & (0b11U << (2 * 7)));
}

void test_gpio_main() {
  UnitySetTestFile(__FILE_NAME__);
  printf("\n\nWIRE TEST_GPIO_PIN_A AND TEST_GPIO_PIN_B\n\n");

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
  RUN_TEST(test_decoupled_GPIO_LockPort);
}
