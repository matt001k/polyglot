/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup watchdog
 * @{
 */

/**
 * @file        watchdog.h
 *
 * @brief       Provides API for the Watchdog Timer
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-14
 */

/*****************************************************************************/
#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#include "platform.h"

typedef void (*Watchdog_Callback_t)(void);

void Watchdog_Init(void);
void Watchdog_Deinit(void);
void Watchdog_Enable(void);
void Watchdog_Disable(void);
void Watchdog_Kick(void);
void Watchdog_RegisterCallback(Watchdog_Callback_t cb);

#endif // __WATCHDOG_H

/**@} watchdog */