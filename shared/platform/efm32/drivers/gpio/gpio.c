/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup gpio
 * @{
 */

/**
 * @file        gpio.c
 *
 * @brief       Provides the API for GPIO
 *
 * @author      Matthew Krause
 *
 * @date        2022-08-25
 */

/*****************************************************************************/
#include "gpio.h"
#include "atomic.h"

#define GPIO_L_MAX (7U)
#define GPIO_H_MAX (15U)

typedef enum
{
    PORTA = 0U,
    PORTB = 1U,
    PORTC = 2U,
    PORTD = 3U,
    PORTE = 4U,
    PORTF = 5U,
    PORTG = 6U,
    PORTH = 7U,
    PORTI = 8U,
    PORTJ = 9U,
    PORTK = 10U,
    PORTL = 11U,
    PORT_NUM,
} gpio_Port_t;

typedef enum
{
    DISABLED = 0U,
    INPUT = 1U,
    INPUTPULL = 2U,
    INPUTPULLFILTER = 3U,
    PUSHPULL = 4U,
    PUSHPULLALT = 5U,
    WIREDOR = 6U,
    WIREDORPULLDOWN = 7U,
    WIREDAND = 8U,
    WIREDANDFILTER = 9U,
    WIREDANDPULLUP = 10U,
    WIREDANDPULLUPFILTER = 11U,
    WIREDANDALT = 12U,
    WIREDANDALTFILTER = 13U,
    WIREDANDALTPULLUP = 14U,
    WIREDANDALTPULLUPFILTER = 15U,
} gpio_Mode_t;

typedef enum
{
    PIN0 = 0U,
    PIN1 = 1U,
    PIN2 = 2U,
    PIN3 = 3U,
    PIN4 = 4U,
    PIN5 = 5U,
    PIN6 = 6U,
    PIN7 = 7U,
    PIN8 = 8U,
    PIN9 = 9U,
    PIN10 = 10U,
    PIN11 = 11U,
    PIN12 = 12U,
    PIN13 = 13U,
    PIN14 = 14U,
    PIN15 = 15U,
} gpio_Pin_t;


typedef struct
{
    gpio_Port_t  port : 4;
    gpio_Pin_t   pin  : 4;
    gpio_Mode_t  mode : 4;
    GPIO_State_t state : 1;
} gpio_PinDefs_t;


static gpio_PinDefs_t pDef[NUM_GPIO] = 
{
    [DEBUG_RX] = {PORTD, PIN1, PUSHPULLALT, GPIO_SET},
    [DEBUG_TX] = {PORTD, PIN0, PUSHPULLALT, GPIO_SET},
    [I2S_BCLK] = {PORTA, PIN2, PUSHPULLALT, GPIO_CLEAR},
    [I2S_FS] = {PORTA, PIN3, PUSHPULLALT, GPIO_SET},
    [I2S_ADCOUT] = {PORTA, PIN1, INPUT, GPIO_CLEAR},
    [I2S_DACIN] = {PORTA, PIN0, PUSHPULLALT, GPIO_CLEAR},
    [I2S_MCLK] = {PORTA, PIN12, PUSHPULLALT, GPIO_CLEAR},
    [I2C_SCLK] = {PORTD, PIN7, WIREDANDALTPULLUP, GPIO_CLEAR},
    [I2C_SDIO] = {PORTD, PIN6, WIREDANDALTPULLUP, GPIO_CLEAR},
    [FLASH_SCLK] = {PORTB, PIN5, PUSHPULLALT, GPIO_CLEAR},
    [FLASH_CS] = {PORTB, PIN6, PUSHPULLALT, GPIO_SET},
    [FLASH_SO] = {PORTB, PIN3, PUSHPULLALT, GPIO_CLEAR},
    [FLASH_SI] = {PORTB, PIN4, INPUT, GPIO_CLEAR},
    [FLASH_WP] = {PORTB, PIN7, PUSHPULL, GPIO_SET},
    [LED1] = {PORTE, PIN10, PUSHPULL, GPIO_CLEAR},
    [LED2] = {PORTE, PIN11, PUSHPULL, GPIO_CLEAR},
    [LED3] = {PORTE, PIN12, PUSHPULL, GPIO_CLEAR},
    [POT1] = {PORTE, PIN6, INPUT, GPIO_CLEAR},
    [POT2] = {PORTE, PIN5, INPUT, GPIO_CLEAR},
    [POT3] = {PORTE, PIN4, INPUT, GPIO_CLEAR},
    [VBIAS_MEAS] = {PORTE, PIN15, INPUT, GPIO_CLEAR},
    [SWITCH_IN] = {PORTE, PIN13, PUSHPULLALT, GPIO_CLEAR},
    [XTAL_P] = {PORTB, PIN13, PUSHPULLALT, GPIO_CLEAR},
    [XTAL_N] = {PORTB, PIN14, PUSHPULLALT, GPIO_CLEAR},
};

void GPIO_Init(void)
{
    /* Enable GPIO Clock */
    CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;

    /* Configure GPIO Pins */
    for (uint8_t gIdx = 0U; gIdx < NUM_GPIO; gIdx++)
    {
        GPIO_SetState(gIdx, pDef[gIdx].state);
        if (pDef[gIdx].pin <= GPIO_L_MAX)
        {
            GPIO->P[pDef[gIdx].port].MODEL |= pDef[gIdx].mode <<
                                              (4U * pDef[gIdx].pin);
        }
        else
        {
            GPIO->P[pDef[gIdx].port].MODEH |= pDef[gIdx].mode <<
                                              (4U * (pDef[gIdx].pin -
                                              GPIO_L_MAX - 1U));
        }
    }

    for (uint8_t pIdx = 0U; pIdx < PORT_NUM; pIdx++)
    {
        GPIO->P[pIdx].CTRL = GPIO_P_CTRL_DRIVESTRENGTH_STRONG |
                             GPIO_P_CTRL_SLEWRATE_DEFAULT |
                             GPIO_P_CTRL_DRIVESTRENGTHALT_STRONG |
                             GPIO_P_CTRL_SLEWRATEALT_DEFAULT;
    }
}

void GPIO_Deinit(void)
{
    ENTER_CRITICAL();
    /* De-configure GPIO Pins */
    for (uint8_t gIdx = 0U; gIdx < NUM_GPIO; gIdx++)
    {
        GPIO_SetState(gIdx, pDef[gIdx].state);
        if (pDef[gIdx].pin <= GPIO_L_MAX)
        {
            GPIO->P[pDef[gIdx].port].MODEL = 0U;         
        }
        else
        {
            GPIO->P[pDef[gIdx].port].MODEH = 0U;
        }
    }

    for (uint8_t pIdx = 0U; pIdx < PORT_NUM; pIdx++)
    {
        GPIO->P[pIdx].CTRL = 0U;
    }

    /* Disable GPIO Clock */
    CMU->HFBUSCLKEN0 &= ~(CMU_HFBUSCLKEN0_GPIO);
    EXIT_CRITICAL();
}

void GPIO_SetState(GPIO_Pin_t pin, GPIO_State_t state)
{
    GPIO->P[pDef[pin].port].DOUT &= ~(1UL << pDef[pin].pin);
    GPIO->P[pDef[pin].port].DOUT |= (state << pDef[pin].pin);
}

void GPIO_Set(GPIO_Pin_t pin)
{
    GPIO->P[pDef[pin].port].DOUT |= (1UL << pDef[pin].pin);
}

void GPIO_Clear(GPIO_Pin_t pin)
{
    GPIO->P[pDef[pin].port].DOUT &= ~(1UL << pDef[pin].pin);
}

void GPIO_Toggle(GPIO_Pin_t pin)
{
    GPIO->P[pDef[pin].port].DOUTTGL |= (1UL << pDef[pin].pin);
}

GPIO_State_t GPIO_Get(GPIO_Pin_t pin)
{
    GPIO_State_t val = (GPIO_State_t) ((GPIO->P[pDef[pin].port].DIN &
                        (1UL << pDef[pin].pin))
                        >> pDef[pin].pin);
    return val;
}

/**@} gpio */