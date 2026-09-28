#include "stm32f446ze.h"
#include <stdint.h>

#ifndef _EXTI_DRIVER_H_
#define _EXTI_DRIVER_H_


typedef enum {
    EXTI_TRIGGER_RISING,
    EXTI_TRIGGER_FALLING,
    EXTI_TRIGGER_RISING_FALLING
} EXTI_Trigger;

typedef enum {
    EXTI_MODE_EVENT,
    EXTI_MODE_INTERRUPT
} EXTI_Mode;

typedef struct {
    uint8_t Port;       // GPIO_PORT_TO_INDEX(GPIOx)
    uint8_t Line;       // 0..15 (identical to GPIO pin index)
    EXTI_Trigger Trigger;
    EXTI_Mode Mode;
} EXTI_Config;

void EXTI_ClockInit(); // TODO
void EXTI_SetConfig(EXTI_Config* Config);
uint32_t EXTI_GetPending();
void EXTI_ClearPending(uint8_t Line);
void EXTI_TriggerSoftwareIE(uint8_t Line);

#endif // _EXTI_DRIVER_H_