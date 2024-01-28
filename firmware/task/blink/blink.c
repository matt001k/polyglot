/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup blink
 * @{
 */

/**************************************************************************//**
 * @file        blink.c
 *
 * @brief       Lays out the blink task to blink required LEDs
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-24
 *****************************************************************************/
#include "blink.h"
#include "led.h"
#include "schedule.h"

BL_STATIC struct
{
    BL_UINT8_T count;
} blink = {0};

BL_STATIC void blink_Run(void);

BL_Err_t Blink_Init(void)
{
    BL_Err_t err = BL_ENODEV;
    BL_STATIC Schedule_Node_t node = {0};

    if (LED_GetCount(&blink.count) == BL_OK)
    {
        err = BL_OK;
        err = Schedule_Add(&node, LED_PERIOD_MIN, blink_Run);
    }

    return err;
}

BL_STATIC void blink_Run(void)
{
    BL_STATIC BL_UINT32_T ms = 0U;
    BL_UINT32_T period = 0U;

    ms += LED_PERIOD_MIN;
    for (BL_UINT8_T lIdx = 0U; lIdx < blink.count; lIdx++)
    {
        if (LED_GetPeriod(lIdx, &period) == BL_OK)
        {
            if (ms % period == 0U)
            {
                LED_Toggle(lIdx);
            }
        }
    }
}

/**@} blink */
