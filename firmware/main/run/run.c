/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup run
 * @{
 */

/**************************************************************************//**
 * @file        run.c
 *
 * @brief       Task scheduler to run the required task queued
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-24
 *****************************************************************************/
#include "run.h"
#include "schedule.h"
#include "systick.h"

BL_Err_t Run(void)
{
    Schedule_Node_t *node = BL_NULL;
    BL_UINT32_T ms = Systick_GetMs();
    BL_Err_t err = BL_ENODEV;

    if (Schedule_GetHead(&node) == BL_OK)
    {
        err = BL_OK;
        do
        {
            Schedule_Run(node, ms);
            Schedule_GetNext(*node, &node);
        } while (node != BL_NULL);
    }

    return err;
}


/**@} run */
