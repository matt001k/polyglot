/******************************************************************************
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup hold
 * @{
 */

/******************************************************************************
 * @file        hold.c
 *
 * @brief       Provides an abstraction layer for the bootloader to hold to the
 *              application
 *
 * @author      Matthew Krause
 *
 * @date        2022-10-29
 *****************************************************************************/
#include "hold.h"

#include "helper.h"

#define HOLD_FUNCTION(hold) hold

typedef BL_BOOL_T (*Hold_Cb_t)(void);

BL_STATIC Hold_Cb_t hold_cb = HOLD_CFG(HOLD_FUNCTION);

BL_Err_t Hold_Get(BL_BOOL_T *ret)
{
  BL_Err_t err = BL_ENODEV;

  if(hold_cb) {
    err  = BL_OK;
    *ret = hold_cb();
  }

  return err;
}

/**@} hold */
