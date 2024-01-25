/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup timer
 * @{
 */

/**
 * @file        timer.h
 *
 * @brief       Provides the API for Timer Driver
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-04
 */

/*****************************************************************************/
#ifndef __TIMER_H
#define __TIMER_H

#include "platform.h"

typedef enum
{
    TIMER_SYSTEM,
    TIMER_DELAY,
    NUM_TIMER,
} Timer_Type_t;

typedef void (*Timer_Callback_t)(uint32_t);

void Timer_Init(void);
void Timer_Deinit(void);
uint32_t Timer_GetFrequency(Timer_Type_t type);
uint32_t Timer_SetFrequency(Timer_Type_t type, uint32_t freq);
void Timer_SetTop(Timer_Type_t type, uint32_t val);
uint32_t Timer_GetCount(Timer_Type_t type);
void Timer_Start(Timer_Type_t type);
void Timer_Stop(Timer_Type_t type);
void Timer_Reset(Timer_Type_t type);
void Timer_RegisterCb(Timer_Type_t type, Timer_Callback_t cb);

#endif // __TIMER_H

/**@} timer */
