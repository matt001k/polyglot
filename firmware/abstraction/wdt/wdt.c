/******************************************************************************
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/
/**
 * @addtogroup wdt
 * @{
 */

/******************************************************************************
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

#define WDT_FUNCTION(init, kick) init, kick,

typedef void (*WDT_Init_t)(void);
typedef void (*WDT_Kick_t)(void);
typedef struct {
  WDT_Init_t init;
  WDT_Kick_t kick;
} WDT_t;

BL_STATIC BL_CONST WDT_t wdt = { WDT_CFG(WDT_FUNCTION) };

BL_Err_t WDT_Init(void)
{
  BL_Err_t err = BL_OK;

  if(wdt.init) {
    wdt.init();
  } else {
    err = BL_EINVAL;
  }

  return err;
}

BL_Err_t WDT_Kick(void)
{
  BL_Err_t err = BL_ENODEV;

  if(wdt.kick) {
    wdt.kick();
    err = BL_OK;
  }

  return err;
}

/**@} wdt */
