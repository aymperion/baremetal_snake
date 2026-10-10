#include "test_config.h"
#include "test_gpio.h"
#include "unity_internals.h"
#include <stdint.h>

extern void initialise_monitor_handles(void);

void _init(void) {}

int main(void) {
  initialise_monitor_handles();
  UnityBegin(__FILE_NAME__);
  if (RUN_TEST_GPIO) {
    test_gpio_main();
  }

  return UnityEnd();
}
