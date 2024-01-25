/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup systick
 * @{
 */

/**************************************************************************//**
 * @file        systick.c
 *
 * @brief       Provides an abstraction layer for the bootloader's serial
 *              ports, which can be anything from a UART port, CAN bus,
 *              SPI bus, I2C bus, etc.
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-21
 *****************************************************************************/
#include "systick.h"

#define SYSTICK_CLK_IDX (0U)
#define SYSTICK_TABLE_ENTRY(init, ms) \
    {init, ms},

typedef struct
{
    Systick_Init_t init;
    Systick_GetMs_t ms;
} systick_Cfg_t;

typedef struct
{
    BL_CONST systick_Cfg_t *cfg;
    BL_UINT8_T count;
} systick_t;

BL_STATIC BL_CONST systick_Cfg_t sCfg[] =
{
    SYSTICK_CFG(SYSTICK_TABLE_ENTRY)
    {0, 0},
};

BL_STATIC systick_t systick = {0U};

BL_Err_t Systick_Init(void)
{
    BL_Err_t err = BL_OK;

    systick.cfg = sCfg;

    /* Initialize the systick module, if there is more than 1 return error */
    while (systick.cfg[systick.count].init != 0 &&
           systick.cfg[systick.count].ms !=0)
    {
        if (systick.cfg[systick.count].init && 
            systick.count < 1)
        {
            systick.cfg[systick.count].init();
            systick.count++;
        }
        else
        {
            err = BL_EINVAL;
            break;
        }
    }

    return err;
}

BL_UINT32_T Systick_GetMs(void)
{
    /* Get the ms from the configured systick peripheral */
    return systick.cfg[SYSTICK_CLK_IDX].ms();
}


/**@} systick */
