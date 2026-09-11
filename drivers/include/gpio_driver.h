#include "stm32f446ze.h"
#include <stdint.h>
#include <sys/types.h>

#ifndef _GPIO_DRIVER_H_
#define _GPIO_DRIVER_H_

typedef enum {
    GPIO_PIN_0  = 0b0000000000000001U,
    GPIO_PIN_1  = 0b0000000000000010U,
    GPIO_PIN_2  = 0b0000000000000100U,
    GPIO_PIN_3  = 0b0000000000001000U,
    GPIO_PIN_4  = 0b0000000000010000U,
    GPIO_PIN_5  = 0b0000000000100000U,
    GPIO_PIN_6  = 0b0000000001000000U,
    GPIO_PIN_7  = 0b0000000010000000U,
    GPIO_PIN_8  = 0b0000000100000000U,
    GPIO_PIN_9  = 0b0000001000000000U,
    GPIO_PIN_10 = 0b0000010000000000U,
    GPIO_PIN_11 = 0b0000100000000000U,
    GPIO_PIN_12 = 0b0001000000000000U,
    GPIO_PIN_13 = 0b0010000000000000U,
    GPIO_PIN_14 = 0b0100000000000000U,
    GPIO_PIN_15 = 0b1000000000000000U
} GPIO_Pin_t;

typedef enum  {
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
    GPIO_PORT_H
} GPIO_Port_t;

typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_ALTFN,
    GPIO_MODE_ANALOG,
    GPIO_MODE_ITR_RT,
    GPIO_MODE_ITR_FT,
    GPIO_MODE_ITR_RFT
} GPIO_Mode_t;

typedef enum {
    GPIO_OTYPE_PUSHPULL,
    GPIO_OTYPE_OPENDRAIN
} GPIO_Output_Type_t;

typedef enum {
    GPIO_OSPEED_LOW,
    GPIO_OSPEED_MEDIUM,
    GPIO_OSPEED_FAST,
    GPIO_OSPEED_HIGH
} GPIO_Output_Speed_t;

typedef enum {
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN,
} GPIO_Pull_t;

typedef enum {
    GPIO_ALTFN_0,
    GPIO_ALTFN_1,
    GPIO_ALTFN_2,
    GPIO_ALTFN_3,
    GPIO_ALTFN_4,
    GPIO_ALTFN_5,
    GPIO_ALTFN_6,
    GPIO_ALTFN_7,
    GPIO_ALTFN_8,
    GPIO_ALTFN_9,
    GPIO_ALTFN_10,
    GPIO_ALTFN_11,
    GPIO_ALTFN_12,
    GPIO_ALTFN_13,
    GPIO_ALTFN_14,
    GPIO_ALTFN_15,    
} GPIO_AltFn_t;


typedef struct {
    GPIOx_RegDef_t* GPIOx;
    GPIO_Pin_t Pin;
    GPIO_Mode_t Mode;
    GPIO_Output_Speed_t Output_Speed;
    GPIO_Output_Type_t Output_Type;
    GPIO_Pull_t Pull;
    GPIO_AltFn_t Alternate_Func;
} GPIO_Init_t;

void GPIO_ClockInit(uint8_t GPIO_Ports); //TODO
void GPIO_Init(GPIO_Init_t* pGPIO_Handle);

void GPIO_DeInit(GPIO_Init_t* pGPIO_Handle); //TODO

bool GPIO_ReadPin(GPIOx_RegDef_t* GPIOx, uint8_t Pin_Number);

void GPIO_SetPin(GPIOx_RegDef_t* GPIOx, uint8_t Pin_Number);

void GPIO_ClearPin(GPIOx_RegDef_t* GPIOx, uint8_t Pin_Number);

void GPIO_TogglePin(GPIOx_RegDef_t* GPIOx, uint8_t Pin_Number);

void GPIO_WritePort(GPIOx_RegDef_t* GPIOx, uint16_t Value);

void GPIO_LockPort(GPIOx_RegDef_t* GPIOx, uint16_t Pin_States);

#endif // _GPIO_DRIVER_H_