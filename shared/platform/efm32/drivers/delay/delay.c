/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup delay
 * @{
 */

/**
 * @file        delay.c
 *
 * @brief       Provides the API for Delay Driver
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-04
 */

/*****************************************************************************/
#include "delay.h"
#include "timer.h"
#include "atomic.h"

#define DELAY_US_PER_MS (1000U)
#define DELAY_US_PER_S (1000000U)

static uint8_t dUsCnt = 0UL;
static uint8_t wait = 0U;

static void delay_Cb(uint32_t count);

void Delay_Init(void)
{
    dUsCnt = Timer_GetFrequency(TIMER_DELAY) / DELAY_US_PER_S;
    Timer_RegisterCb(TIMER_DELAY, delay_Cb);
}

void Delay_Deinit(void)
{

}

void Delay_Ms(uint32_t ms)
{
    uint32_t wTemp = wait = 1U;
    Timer_SetTop(TIMER_DELAY, (dUsCnt * DELAY_US_PER_MS * ms) - 1U);
    Timer_Start(TIMER_DELAY);
    while (wTemp) 
    {
        ENTER_CRITICAL();
        wTemp = wait;
        EXIT_CRITICAL();
    };
}

void Delay_Us(uint32_t us)
{
    uint32_t wTemp = wait = 1U;
    Timer_SetTop(TIMER_DELAY, (dUsCnt * us) - 1U);
    Timer_Start(TIMER_DELAY);
    while (wTemp)
    {
        ENTER_CRITICAL();
        wTemp = wait;
        EXIT_CRITICAL();
    };
}

static void delay_Cb(uint32_t count)
{
    wait = 0U;
}

/**@} delay */