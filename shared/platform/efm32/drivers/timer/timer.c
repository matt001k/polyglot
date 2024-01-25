/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup timer
 * @{
 */

/**
 * @file        timer.c
 *
 * @brief       Provides the API for Timer Driver
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-04
 */

/*****************************************************************************/
#include "timer.h"
#include "clock.h"
#include "atomic.h"

#include <stddef.h>

#define TIMER_SYSTEM_ONE_SHOT (0U)
#define TIMER_SYSTEM_IRQ_PRIORITY (0U)
#define TIMER_SYSTEM_IRQ_ENABLE (1U)

#define TIMER_DELAY_ONE_SHOT (1U)
#define TIMER_DELAY_IRQ_PRIORITY (4U)
#define TIMER_DELAY_IRQ_ENABLE (1U)


#define TIMER_DIV1 (1U)
#define TIMER_DIV2 (2U)
#define TIMER_DIV4 (4U)
#define TIMER_DIV8 (8U)
#define TIMER_DIV16 (16U)
#define TIMER_DIV32 (32U)
#define TIMER_DIV64 (64U)
#define TIMER_DIV128 (128U)
#define TIMER_DIV256 (256U)
#define TIMER_DIV512 (512U)
#define TIMER_DIV1024 (1024U)

typedef struct
{
    uint8_t priority;
    uint8_t enable : 1U;
} timer_IRQ_t;

typedef struct
{
    TIMER_TypeDef *timer;
    uint8_t divider : 4U;
    uint8_t mode : 2U;
    uint8_t oneshot: 1U;
    timer_IRQ_t irq;
    Timer_Callback_t cb;
} timer_Config_t;

static timer_Config_t tCfg[] =
{
    [TIMER_SYSTEM] =
    {
        WTIMER1,
        _TIMER_CTRL_PRESC_DIV4,
        _TIMER_CTRL_MODE_UP,
        TIMER_SYSTEM_ONE_SHOT,
        {TIMER_SYSTEM_IRQ_PRIORITY, TIMER_SYSTEM_IRQ_ENABLE},
        NULL,
    },
    [TIMER_DELAY] =
    {
        WTIMER0,
        _TIMER_CTRL_PRESC_DIV1,
        _TIMER_CTRL_MODE_UP,
        TIMER_DELAY_ONE_SHOT,
        {TIMER_DELAY_IRQ_PRIORITY, TIMER_DELAY_IRQ_ENABLE},
        NULL,
    }
};

static const uint16_t tDivLUT[] =
{
    [_TIMER_CTRL_PRESC_DIV1] = TIMER_DIV1,
    [_TIMER_CTRL_PRESC_DIV2] = TIMER_DIV2,
    [_TIMER_CTRL_PRESC_DIV4] = TIMER_DIV4,
    [_TIMER_CTRL_PRESC_DIV8] = TIMER_DIV8,
    [_TIMER_CTRL_PRESC_DIV16] = TIMER_DIV16,
    [_TIMER_CTRL_PRESC_DIV32] = TIMER_DIV32,
    [_TIMER_CTRL_PRESC_DIV64] = TIMER_DIV64,
    [_TIMER_CTRL_PRESC_DIV128] = TIMER_DIV128,
    [_TIMER_CTRL_PRESC_DIV256] = TIMER_DIV256,
    [_TIMER_CTRL_PRESC_DIV512] = TIMER_DIV512,
    [_TIMER_CTRL_PRESC_DIV1024] = TIMER_DIV1024,
};

void Timer_Init(void)
{
    IRQn_Type irq = 0U;
    for (uint8_t tIdx = 0U; tIdx < NUM_TIMER; tIdx++)
    {
        ENTER_CRITICAL();
        /* Enable the correct clock and set the IRQ enum to be used */
        if (tCfg[tIdx].timer == TIMER0)
        {
            CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER0;
            irq = TIMER0_IRQn;
        }
        else if (tCfg[tIdx].timer == TIMER1)
        {
            CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER1;
            irq = TIMER1_IRQn;
        }
        else if (tCfg[tIdx].timer == WTIMER0)
        {
            CMU->HFPERCLKEN1 |= CMU_HFPERCLKEN1_WTIMER0;
            irq = WTIMER0_IRQn;
        }
        else
        {
            CMU->HFPERCLKEN1 |= CMU_HFPERCLKEN1_WTIMER1;
            irq = WTIMER1_IRQn;
        }

        /* Set the control register, sets initial prescaler */
        tCfg[tIdx].timer->CTRL = tCfg[tIdx].divider <<
                                 _TIMER_CTRL_PRESC_SHIFT |
                                 tCfg[tIdx].oneshot <<
                                 _TIMER_CTRL_OSMEN_SHIFT |
                                 tCfg[tIdx].mode << _TIMER_CTRL_MODE_SHIFT;

        /* Clear count and top registers */
        tCfg[tIdx].timer->CNT = _TIMER_CNT_RESETVALUE;
        tCfg[tIdx].timer->TOP = _TIMER_TOP_RESETVALUE;

        /* Enables IRQ */
        if (tCfg[tIdx].irq.enable)
        {
            if (tCfg[tIdx].mode == _TIMER_CTRL_MODE_UP)
            {
                tCfg[tIdx].timer->IEN = TIMER_IEN_OF;
            }
            else
            {
                tCfg[tIdx].timer->IEN = TIMER_IEN_UF;
            }

            NVIC_ClearPendingIRQ(irq);
            NVIC_SetPriority(irq, tCfg[tIdx].irq.priority);
            NVIC_EnableIRQ(irq);
        }
        EXIT_CRITICAL();
    }   
}

void Timer_Deinit(void)
{
    IRQn_Type irq = 0U;
    for (uint8_t tIdx = 0U; tIdx < NUM_TIMER; tIdx++)
    {
        ENTER_CRITICAL();
        Timer_Stop((Timer_Type_t) tIdx);

        tCfg[tIdx].timer->CTRL = 0U;
        tCfg[tIdx].timer->CNT = 0U;
        tCfg[tIdx].timer->TOP = 0U;

        /* Disable the correct clock and the IRQ enum to be used */
        if (tCfg[tIdx].timer == TIMER0)
        {
            CMU->HFPERCLKEN0 &= ~(CMU_HFPERCLKEN0_TIMER0);
            irq = TIMER0_IRQn;
        }
        else if (tCfg[tIdx].timer == TIMER1)
        {
            CMU->HFPERCLKEN0 &= ~(CMU_HFPERCLKEN0_TIMER1);
            irq = TIMER1_IRQn;
        }
        else if (tCfg[tIdx].timer == WTIMER0)
        {
            CMU->HFPERCLKEN1 &= ~(CMU_HFPERCLKEN1_WTIMER0);
            irq = WTIMER0_IRQn;
        }
        else
        {
            CMU->HFPERCLKEN1 &= ~(CMU_HFPERCLKEN1_WTIMER1);
            irq = WTIMER1_IRQn;
        }
        NVIC_ClearPendingIRQ(irq);
        NVIC_DisableIRQ(irq);
        EXIT_CRITICAL();
    }
}

uint32_t Timer_GetFrequency(Timer_Type_t type)
{
    uint32_t frequency = 0U;

    /* Gets the specified timer frequency */
    if (type < NUM_TIMER)
    {
        ENTER_CRITICAL();
        frequency = Clock_GetFrequency(0U) / tDivLUT[tCfg[type].divider];
        EXIT_CRITICAL();
    }

    return frequency;
}

uint32_t Timer_SetFrequency(Timer_Type_t type, uint32_t freq)
{
    uint32_t clk = Clock_GetFrequency(0U);
    uint8_t div = _TIMER_CTRL_PRESC_DIV1;
    uint32_t ret = clk;

    if (type < NUM_TIMER && freq <= clk)
    {
        /* Sets the requested frequency dependant on the allowed prescalers */
        if (freq <= clk / TIMER_DIV1024)
        {
            div = _TIMER_CTRL_PRESC_DIV1024;
            ret = clk / TIMER_DIV1024;
        }
        else if (freq <= clk / TIMER_DIV512)
        {
            div = _TIMER_CTRL_PRESC_DIV512;
            ret = clk / TIMER_DIV1024;
        }
        else if (freq <= clk / TIMER_DIV256)
        {
            div = _TIMER_CTRL_PRESC_DIV256;
            ret = clk / TIMER_DIV1024;
        }
        else if (freq <= clk / TIMER_DIV128)
        {
            div = _TIMER_CTRL_PRESC_DIV128;
            ret = clk / TIMER_DIV128;
        }
        else if (freq <= clk / TIMER_DIV64)
        {
            div = _TIMER_CTRL_PRESC_DIV64;
            ret = clk / TIMER_DIV64;
        }
        else if (freq <= clk / TIMER_DIV32)
        {
            div = _TIMER_CTRL_PRESC_DIV32;
            ret = clk / TIMER_DIV32;
        }
        else if (freq <= clk / TIMER_DIV16)
        {
            div = _TIMER_CTRL_PRESC_DIV16;
            ret = clk / TIMER_DIV16;
        }
        else if (freq <= clk / TIMER_DIV8)
        {
            div = _TIMER_CTRL_PRESC_DIV8;
            ret = clk / TIMER_DIV8;
        }
        else if (freq <= clk / TIMER_DIV4)
        {
            div = _TIMER_CTRL_PRESC_DIV4;
            ret = clk / TIMER_DIV4;
        }
        else if (freq <= clk / TIMER_DIV2)
        {
            div = _TIMER_CTRL_PRESC_DIV2;
            ret = clk / TIMER_DIV2;
        }
        ENTER_CRITICAL();
        tCfg[type].timer->CTRL &= ~(_TIMER_CTRL_PRESC_MASK);
        tCfg[type].timer->CTRL |= div <<
                                 _TIMER_CTRL_PRESC_SHIFT;
        EXIT_CRITICAL();
    }

    return ret;
}

void Timer_SetTop(Timer_Type_t type, uint32_t val)
{
    /* Sets the correct top value the timer will count up/down to */
    if (type < NUM_TIMER)
    {
        ENTER_CRITICAL();
        tCfg[type].timer->TOP = val;
        EXIT_CRITICAL();
    }
}
uint32_t Timer_GetCount(Timer_Type_t type)
{
    uint32_t count = 0;

    if (type < NUM_TIMER)
    {
        ENTER_CRITICAL();
        count = tCfg[type].timer->CNT;
        EXIT_CRITICAL();
    }

    return count;
}

void Timer_Start(Timer_Type_t type)
{
    if (type < NUM_TIMER)
    {
        ENTER_CRITICAL();
        tCfg[type].timer->CMD = TIMER_CMD_START;
        EXIT_CRITICAL();
    }
}

void Timer_Stop(Timer_Type_t type)
{
    if (type < NUM_TIMER)
    {
        ENTER_CRITICAL();
        tCfg[type].timer->CMD = TIMER_CMD_STOP;
        EXIT_CRITICAL();
    }
}

void Timer_Reset(Timer_Type_t type)
{
    if (type < NUM_TIMER)
    {
        ENTER_CRITICAL();
        tCfg[type].timer->CNT = _TIMER_CNT_RESETVALUE;
        EXIT_CRITICAL();
    }
}

void Timer_RegisterCb(Timer_Type_t type, Timer_Callback_t cb)
{
    if (type < NUM_TIMER && cb)
    {
        ENTER_CRITICAL();
        tCfg[type].cb = cb;
        EXIT_CRITICAL();
    }
}

void WTIMER1_IRQHandler(void)
{
    static uint32_t count = 0U;

    count++;
    if (tCfg[TIMER_SYSTEM].cb)
    {
        tCfg[TIMER_SYSTEM].cb(count);
    }

    if (tCfg[TIMER_SYSTEM].mode == _TIMER_CTRL_MODE_UP)
    {
        tCfg[TIMER_SYSTEM].timer->IFC |= TIMER_IFC_OF;
    }
    else
    {
        tCfg[TIMER_SYSTEM].timer->IFC |= TIMER_IFC_UF;
    }
}

void WTIMER0_IRQHandler(void)
{
    static uint32_t count = 0U;

    count++;
    if (tCfg[TIMER_DELAY].cb)
    {
        tCfg[TIMER_DELAY].cb(count);
    }

    if (tCfg[TIMER_DELAY].mode == _TIMER_CTRL_MODE_UP)
    {
        tCfg[TIMER_DELAY].timer->IFC |= TIMER_IFC_OF;
    }
    else
    {
        tCfg[TIMER_DELAY].timer->IFC |= TIMER_IFC_UF;
    }
}

/**@} timer */
