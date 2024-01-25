/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/
/**
 * @addtogroup wdt
 * @{
 */

/**************************************************************************//**
 * @file        wdt.h
 *
 * @brief       Provides an abstraction layer for the bootloader's watchdog
 *              timer
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "wdt.h"

#define WDT_FUNCTION(init, kick) \
    {init, kick},

typedef struct
{
    WDT_Init_t init;
    WDT_Kick_t kick;
} wdt_Cfg_t;

typedef struct
{
    BL_CONST wdt_Cfg_t *cfg;
    BL_UINT8_T count;
} wdt_t;

BL_STATIC BL_CONST wdt_Cfg_t wCfg[] =
{
    WDT_CFG(WDT_FUNCTION)
    {0, 0},
};

BL_STATIC wdt_t wdt = {0};

BL_Err_t WDT_Init(void)
{
    BL_Err_t err = BL_OK;

    wdt.cfg = wCfg;

    while (wdt.cfg->init != 0 &&
           wdt.cfg->kick != 0)
    {
        if (wdt.cfg[wdt.count].init && wdt.count < 1U)
        {
            wdt.cfg[wdt.count].init();
            wdt.count++;
        }
        else
        {
            err = BL_EINVAL;
            break;
        }
    }
    
    return err;
}

BL_Err_t WDT_Kick(void)
{
    BL_Err_t err = BL_ENODEV;

    if (wdt.cfg[wdt.count].kick)
    {
        wdt.cfg[wdt.count].kick();
        err = BL_OK;
    }

    return err;
}

/**@} wdt */
