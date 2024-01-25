/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup time
 * @{
 */

/**
 * @file        time.c
 *
 * @brief       Provides the API for time interface
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-21
 */

/*****************************************************************************/
#include "time.h"
#include "timer.h"

#define MS_TO_S (1000U)
#define TICK_TO_MS() ((Timer_GetFrequency(TIMER_SYSTEM) / MS_TO_S) - 1U)

static void time_Cb(uint32_t tick);
static uint32_t ms = 0U;

void Time_Init(void)
{
    Timer_RegisterCb(TIMER_SYSTEM, time_Cb);
    Timer_SetTop(TIMER_SYSTEM, TICK_TO_MS());
    Timer_Start(TIMER_SYSTEM);
}

uint32_t Time_GetRuntimeMs(void)
{
    return ms;
}

uint32_t Time_GetRuntimeS(void)
{
    return ms / MS_TO_S;
}

static void time_Cb(uint32_t tick)
{
    ms++;
}

/**@} time */
