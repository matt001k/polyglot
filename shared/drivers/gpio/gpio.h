/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup gpio
 * @{
 */

/**
 * @file        gpio.h
 *
 * @brief       Provides the API for GPIO
 *
 * @author      Matthew Krause
 *
 * @date        2022-08-25
 */

/*****************************************************************************/
#ifndef __GPIO_H
#define __GPIO_H

#include "platform.h"

typedef uint8_t GPIO_Port_t;

typedef enum
{
    DEBUG_RX = 0U,
    DEBUG_TX,
    I2S_BCLK,
    I2S_FS,
    I2S_ADCOUT,
    I2S_DACIN,
    I2S_MCLK,
    I2C_SCLK,
    I2C_SDIO,
    FLASH_SCLK,
    FLASH_CS,
    FLASH_SO,
    FLASH_SI,
    FLASH_WP,
    LED1,
    LED2,
    LED3,
    POT1,
    POT2,
    POT3,
    VBIAS_MEAS,
    SWITCH_IN,
    XTAL_P,
    XTAL_N,
    NUM_GPIO,
} GPIO_Pin_t;

typedef struct
{
    GPIO_Port_t port;
    GPIO_Pin_t pin;
} GPIO_t;


typedef enum
{
    GPIO_CLEAR = 0U,
    GPIO_SET = 1U,
} GPIO_State_t;

void GPIO_Init(void);
void GPIO_Deinit(void);
void GPIO_SetState(GPIO_Pin_t pin, GPIO_State_t state);
void GPIO_Set(GPIO_Pin_t pin);
void GPIO_Clear(GPIO_Pin_t pin);
void GPIO_Toggle(GPIO_Pin_t pin);
GPIO_State_t GPIO_Get(GPIO_Pin_t pin);
GPIO_Pin_t GPIO_GetPin(GPIO_Pin_t pin);
GPIO_Port_t GPIO_GetPort(GPIO_Pin_t pin);

/**@} gpio */

#endif //__GPIO_H