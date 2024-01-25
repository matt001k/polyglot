/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup watchdog
 * @{
 */

/**
 * @file        watchdog.c
 *
 * @brief       Provides API for the Watchdog Timer
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-14
 */

/*****************************************************************************/
#include "watchdog.h"
#include "atomic.h"

#define WATCHDOG_TIMEOUT_PERSEL (13U << _WDOG_CTRL_PERSEL_SHIFT)
#define WATCHDOG_WARNSEL_75 (3U << _WDOG_CTRL_WARNSEL_SHIFT)
#define WATCHDOG_IRQ_PRIORITY (0U)

static Watchdog_Callback_t wCb = NULL;

void Watchdog_Init(void)
{
    /* Enable the watchdog warning interrupt */
    WDOG0->IEN = WDOG_IEN_WARN;
    NVIC_ClearPendingIRQ(WDOG0_IRQn);
    NVIC_SetPriority(WDOG0_IRQn, WATCHDOG_IRQ_PRIORITY);
    NVIC_EnableIRQ(WDOG0_IRQn);

    /* Configure the watchdog timer with a timeout of 2 seconds */
    while (WDOG0->SYNCBUSY & WDOG_SYNCBUSY_CTRL) {};
    WDOG0->CTRL = WDOG_CTRL_CLKSEL_LFRCO | WATCHDOG_TIMEOUT_PERSEL |
                  WATCHDOG_WARNSEL_75;
}

void Watchdog_Deinit(void)
{
    ENTER_CRITICAL();
    Watchdog_Disable();
    WDOG0->IEN = 0U;
    NVIC_ClearPendingIRQ(WDOG0_IRQn);
    NVIC_DisableIRQ(WDOG0_IRQn);
    WDOG0->CTRL = 0U;
    EXIT_CRITICAL();
}

void Watchdog_Enable(void)
{
    /* Enable the watchdog timer */
    WDOG0->CTRL |= WDOG_CTRL_EN;
}

void Watchdog_Disable(void)
{
    /* Disable the watchdog timer */
    WDOG0->CTRL &= ~WDOG_CTRL_EN;
}

void Watchdog_Kick(void)
{
    WDOG0->CMD = WDOG_CMD_CLEAR_CLEARED;
}

void Watchdog_RegisterCallback(Watchdog_Callback_t cb)
{
    if (cb)
    {
        wCb = cb;
    }
}

void WDOG0_IRQHandler(void)
{
    if (wCb)
    {
        wCb();
    }
    WDOG0->IFC = WDOG_IFC_WARN;
}

/**@} watchdog */