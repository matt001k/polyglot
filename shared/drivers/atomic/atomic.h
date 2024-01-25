/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup atomic
 * @{
 */

/**
 * @file        atomic.h
 *
 * @brief       Provides the API for atomic operations
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-21
 */

/*****************************************************************************/
#ifndef __ATOMIC_H
#define __ATOMIC_H

#include "platform.h"

#define ENTER_CRITICAL() do { uint32_t cState = Atomic_EnterCritical()
#define EXIT_CRITICAL() Atomic_ExitCritical(cState); } while(0U)

uint32_t Atomic_EnterCritical(void);
void Atomic_ExitCritical(uint32_t state);

#endif // __ATOMIC_H

/**@} atomic */