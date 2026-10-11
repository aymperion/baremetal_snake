#include "test_config.h"
#include "test_gpio.h"
#include "test_spi.h"
#include "unity_internals.h"
#include <stdint.h>

extern void initialise_monitor_handles(void);

void (*g_current_suite_setUp)(void) = NULL;
void (*g_current_suite_tearDown)(void) = NULL;

void setUp(void) {
  if (g_current_suite_setUp) {
    g_current_suite_setUp();
  }
}
void tearDown(void) {
  if (g_current_suite_tearDown) {
    g_current_suite_tearDown();
  }
}

void _init(void) {}

int main(void) {
  initialise_monitor_handles();

#if RUN_TEST_GPIO
    test_gpio_main();
#endif

#if RUN_TEST_SPI
    test_SPI_main();
#endif

  return UnityEnd();
}
