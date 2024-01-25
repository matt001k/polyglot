/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup hold
 * @{
 */

/**************************************************************************//**
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

#define HOLD_IDX (0U)
#define HOLD_FUNCTION(hold) \
    {hold},

typedef struct
{
    Hold_Cb_t cb;
} hold_Cfg_t;

typedef struct
{
    BL_CONST hold_Cfg_t *cfg;
    BL_UINT8_T count;
} hold_t;

BL_STATIC BL_CONST hold_Cfg_t hCfg[] =
{
    HOLD_CFG(HOLD_FUNCTION)
    {0},
};

BL_STATIC hold_t hold = {0U};

BL_Err_t Hold_Init(void)
{
    BL_Err_t err = BL_OK;

    hold.cfg = hCfg;

    while (hold.cfg[hold.count].cb != 0)
    {
        hold.count++;
    }

    if (hold.count != 1)
    {
        err = BL_EINVAL;
    }

    return err;
}

BL_Err_t Hold_Get(BL_BOOL_T *ret)
{
    BL_Err_t err = BL_ENODEV;

    if (hold.cfg[HOLD_IDX].cb)
    {
        err = BL_OK;
        *ret = hold.cfg[HOLD_IDX].cb();
    }

    return err;
}

/**@} hold */
