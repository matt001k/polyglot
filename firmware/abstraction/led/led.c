/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup led
 * @{
 */

/**************************************************************************//**
 * @file        led.c
 *
 * @brief       Provides tan abstraction layer for the bootloader's led
 *              interface
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-19
 *****************************************************************************/
#include "led.h"

#define LED_TABLE_ENTRY(toggle, period) \
    {toggle, period},

typedef struct
{
    LED_Toggle_t toggle;
    BL_UINT32_T period;
} led_Cfg_t;

typedef struct
{
    BL_CONST led_Cfg_t *cfg;
    BL_LED_T count;
} led_t;

BL_STATIC BL_CONST led_Cfg_t lCfg[] =
{
    LED_CFG(LED_TABLE_ENTRY)
    {0, 0},
};

BL_STATIC led_t led = {0U};

BL_Err_t LED_Init(void)
{
    BL_Err_t err = BL_OK;

    led.cfg = lCfg;

    while (led.cfg[led.count].toggle != 0 &&
           led.cfg[led.count].period != 0)
    {
        if (led.cfg->period < LED_PERIOD_MIN ||
            led.cfg->period % LED_PERIOD_MIN != 0U)
        {
            err = BL_EINVAL;
        }
        led.count++;
    }

    return err;
}

BL_Err_t LED_GetCount(BL_UINT8_T *count)
{
    BL_Err_t err = BL_EINVAL;

    if (count)
    {
        err = BL_OK;
        *count = led.count;
    }

    return err;
}

BL_Err_t LED_GetPeriod(BL_LED_T num, BL_UINT32_T *period)
{
    BL_Err_t err = BL_EINVAL;

    if (num < led.count && period)
    {
        err = BL_OK;
        *period = led.cfg[num].period;
    }

    return err;
}

BL_Err_t LED_Toggle(BL_LED_T num)
{
    BL_Err_t err = BL_EINVAL;

    if (num < led.count)
    {
        err = BL_OK;
        led.cfg[num].toggle();
    }

    return err;
}

/**@} led */
