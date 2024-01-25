/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup atomic
 * @{
 */

/**
 * @file        atomic.c
 *
 * @brief       Provides the API for atomic operations
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-21
 */

/*****************************************************************************/
#include "atomic.h"

uint32_t Atomic_EnterCritical(void)
{
    uint32_t state = __get_PRIMASK();

  __disable_irq();

  return state;
}

void Atomic_ExitCritical(uint32_t state)
{
    if (state == 0U)
    {
        __enable_irq();
    }
}

/**@} atomic */