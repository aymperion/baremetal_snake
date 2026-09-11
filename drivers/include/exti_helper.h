#include "stm32f446ze.h"
#include <stdint.h>

#ifndef _EXTI_DRIVER_H_
#define _EXTI_DRIVER_H_

typedef enum {
    EXTI_GPIOA = 0b0000U,
    EXTI_GPIOB = 0b0001U,
    EXTI_GPIOC = 0b0010U,
    EXTI_GPIOD = 0b0011U,
    EXTI_GPIOE = 0b0100U,
    EXTI_GPIOF = 0b0101U,
    EXTI_GPIOG = 0b0110U,
    EXTI_GPIOH = 0b0111U
} EXTI_GPIO;

typedef enum {
    EXTI_LINE0,
    EXTI_LINE1,
    EXTI_LINE2,
    EXTI_LINE3,
    EXTI_LINE4,
    EXTI_LINE5,
    EXTI_LINE6,
    EXTI_LINE7,
    EXTI_LINE8,
    EXTI_LINE9,
    EXTI_LINE10,
    EXTI_LINE11,
    EXTI_LINE12,
    EXTI_LINE13,
    EXTI_LINE14,
    EXTI_LINE15
} EXTI_Line;

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
    EXTI_GPIO GPIOx;
    EXTI_Line Line;
    EXTI_Trigger Trigger;
    EXTI_Mode Mode;
} EXTI_Config;

void EXTI_ClockInit(); // TODO
void EXTI_SetConfig(EXTI_Config* Config);
uint32_t EXTI_GetPending();
void EXTI_ClearPending(EXTI_Line Line);
void EXTI_TriggerSoftwareIE(EXTI_Line Line);

#endif // _EXTI_DRIVER_H_