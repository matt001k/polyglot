/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup uart
 * @{
 */

/**
 * @file        uart.c
 *
 * @brief       Provides the API for UART Driver
 *
 * @author      Matthew Krause
 *
 * @date        2022-08-25
 */

/*****************************************************************************/
#include "uart.h"
#include "atomic.h"
#include "clock.h"
#include "dma.h"
#include "gpio.h"

#include <stddef.h>

#define UART_BUF_SIZE (256U)
#define UART_TX_READY (1U)
#define UART_TX_NOT_READY (0U)
#define UART_BAUD_CALC(baud, os, var)                       \
do                                                          \
{                                                           \
    var = 32U * Clock_GetFrequency(0U) + (os * baud) / 2U;  \
    var /= os * baud;                                       \
    var -= 32U;                                             \
    var *= 8U;                                              \
} while (0U)

#define UART_DEBUG_BAUD (115200U)
#define UART_DEBUG_IRQ_PRIORITY (4U)

typedef uint32_t uart_Location_t;

typedef enum
{
    OVERSAMPLE_16 = 0U,
    OVERSAMPLE_8 = 1U,
    OVERSAMPLE_6 = 2U,
    OVERSAMPLE_4 = 3U,
} uart_OVS_t;

typedef struct
{
    USART_TypeDef *uart;
    uint32_t baud;
    uart_Location_t rx;
    uart_Location_t tx;
    DMA_Channel_t channel;
    uint8_t irqPriority;
    uint8_t txReady;
    DMA_Callback_t dmaCb;
    UART_TxCallback_t txCb;
    UART_RxCallback_t rxCb;
    uint8_t txBuf[UART_BUF_SIZE];
} uart_Defs_t;

static const uint8_t uOVSLUT[] =
{
    [OVERSAMPLE_16] = 16U,
    [OVERSAMPLE_8] = 8U,
    [OVERSAMPLE_6] = 6U,
    [OVERSAMPLE_4] = 4U,
};

static void uDebugDMACb(void *arg);

static uart_Defs_t uCfg[NUM_UART] =
{
    [UART_DEBUG] =
    {
        USART1,
        UART_DEBUG_BAUD,
        USART_ROUTELOC0_RXLOC_LOC1,
        USART_ROUTELOC0_TXLOC_LOC1,
        UART_DEBUG_DMA,
        UART_DEBUG_IRQ_PRIORITY,
        UART_TX_READY,
        uDebugDMACb,
        NULL,
        NULL,
    },
};

void UART_Init(void)
{
    uint32_t uClockSelect = CMU_HFPERCLKEN0_USART0;
    uint32_t uClkDiv = 0UL;
    IRQn_Type irq = 0U;

    for (uint8_t uIdx = 0U; uIdx < NUM_UART; uIdx++)
    {
        ENTER_CRITICAL();
        if(uCfg[uIdx].uart == USART0)
        {
            uClockSelect = CMU_HFPERCLKEN0_USART0;
            irq = USART0_IRQn;
        }
        else if(uCfg[uIdx].uart == USART1)
        {
            uClockSelect = CMU_HFPERCLKEN0_USART1;
            irq = USART1_IRQn;
        }
        else if(uCfg[uIdx].uart == USART2)
        {
            uClockSelect = CMU_HFPERCLKEN0_USART2;
            irq = USART2_IRQn;
        }
        else
        {
            uClockSelect = CMU_HFPERCLKEN0_USART3;
            irq = USART3_IRQn;
        }
        CMU->HFPERCLKEN0 |= uClockSelect;

        uCfg[uIdx].uart->CTRL = USART_CTRL_OVS_X16 | USART_CTRL_SYNC_DEFAULT;
        uCfg[uIdx].uart->FRAME = USART_FRAME_DATABITS_EIGHT |
                                USART_FRAME_PARITY_NONE |
                                USART_FRAME_STOPBITS_ONE;
        UART_BAUD_CALC(uCfg[uIdx].baud, uOVSLUT[OVERSAMPLE_16], uClkDiv);
        uCfg[uIdx].uart->CLKDIV =  uClkDiv;
        uCfg[uIdx].uart->TIMING = _USART_TIMING_RESETVALUE;
        uCfg[uIdx].uart->ROUTELOC0 = uCfg[uIdx].rx | uCfg[uIdx].tx;
        uCfg[uIdx].uart->ROUTEPEN = USART_ROUTEPEN_RXPEN | UART_ROUTEPEN_TXPEN;
        
        uCfg[uIdx].uart->IEN = USART_IEN_RXDATAV;
        NVIC_ClearPendingIRQ(irq);
        NVIC_SetPriority(irq, uCfg[uIdx].irqPriority);
        NVIC_EnableIRQ(irq);

        UART_Flush(uIdx);

        uCfg[uIdx].uart->CMD = USART_CMD_RXEN | USART_CMD_TXEN;
        while (!(uCfg[uIdx].uart->STATUS &
              (USART_STATUS_TXENS | USART_STATUS_RXENS))) {};

        DMA_RegisterCb(uCfg[uIdx].channel, uCfg[uIdx].dmaCb, NULL);
        EXIT_CRITICAL();
    }
}

void UART_Deinit(void)
{
    uint32_t uClockSelect = CMU_HFPERCLKEN0_USART0;
    IRQn_Type irq = 0U;

    for (uint8_t uIdx = 0U; uIdx < NUM_UART; uIdx--)
    {
        ENTER_CRITICAL();
        if(uCfg[uIdx].uart == USART0)
        {
            uClockSelect = CMU_HFPERCLKEN0_USART0;
            irq = USART0_IRQn;
        }
        else if(uCfg[uIdx].uart == USART1)
        {
            uClockSelect = CMU_HFPERCLKEN0_USART1;
            irq = USART1_IRQn;
        }
        else if(uCfg[uIdx].uart == USART2)
        {
            uClockSelect = CMU_HFPERCLKEN0_USART2;
            irq = USART2_IRQn;
        }
        else
        {
            uClockSelect = CMU_HFPERCLKEN0_USART3;
            irq = USART3_IRQn;
        }

        /* Disable interrupts and de-config the peripheral */
        NVIC_ClearPendingIRQ(irq);
        NVIC_DisableIRQ(irq);
        uCfg[uIdx].uart->IEN = 0U;
        uCfg[uIdx].uart->CTRL = 0U;
        uCfg[uIdx].uart->FRAME = 0U;
        uCfg[uIdx].uart->TRIGCTRL = 0U;
        uCfg[uIdx].uart->CLKDIV = 0U;
        uCfg[uIdx].uart->IEN = 0U;
        uCfg[uIdx].uart->IFC = 0U;
        uCfg[uIdx].uart->TIMING = 0U;
        uCfg[uIdx].uart->ROUTEPEN = 0U;
        uCfg[uIdx].uart->ROUTELOC0 = 0U;
        uCfg[uIdx].uart->ROUTELOC1 = 0U;
        CMU->HFPERCLKEN0 &= ~(uClockSelect);
        EXIT_CRITICAL();
    }
}

void UART_Transmit(UART_Type_t type, uint8_t *data, uint32_t size)
{
    /* Ensure the transmission is valid and another is not ongoing */
    if (type < NUM_UART && data && size)
    {
        while (!uCfg[type].txReady) {};
        size = size < UART_BUF_SIZE ? size : UART_BUF_SIZE;
        memcpy(uCfg[type].txBuf, data, size);
        DMA_StartTransfer(uCfg[type].channel, uCfg[type].txBuf, size);
        uCfg[type].txReady = UART_TX_NOT_READY;
    }
}

void UART_Receive(UART_Type_t type, uint8_t *data, uint32_t size)
{
    if (type < NUM_UART && data && size)
    {
        while (size--)
        {
            while (!(uCfg[type].uart->STATUS & USART_STATUS_RXDATAV)) {};
            *data++ = uCfg[UART_DEBUG].uart->RXDATA;
        }
    }
}

void UART_RegisterTxCallback(UART_Type_t type, UART_TxCallback_t cb)
{
    if (type < NUM_UART && cb)
    {
        uCfg[type].txCb = cb;
    }
}

void UART_RegisterRxCallback(UART_Type_t type, UART_RxCallback_t cb)
{
    if (type < NUM_UART && cb)
    {
        uCfg[type].rxCb = cb;
    }
}

void UART_DeregisterTxCallback(UART_Type_t type)
{
    if (type < NUM_UART && uCfg[type].txCb)
    {
        uCfg[type].txCb = NULL;
    }
}

void UART_DeregisterRxCallback(UART_Type_t type)
{
    if (type < NUM_UART && uCfg[type].rxCb)
    {
        uCfg[type].rxCb = NULL;
    }
}

void UART_Flush(UART_Type_t type)
{
    uCfg[type].uart->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
    while (!(uCfg[type].uart->STATUS &
           (USART_STATUS_TXBL | USART_STATUS_RXDATAV))) {};
}

void USART1_IRQHandler(void)
{
    static uint8_t uData = 0U;

    if (uCfg[UART_DEBUG].uart->IF & USART_IF_RXDATAV)
    {
        uData = uCfg[UART_DEBUG].uart->RXDATA;
        if (uCfg[UART_DEBUG].rxCb)
        {
            uCfg[UART_DEBUG].rxCb(&uData, sizeof(uData));
        }
        uCfg[UART_DEBUG].uart->IFC |= USART_IF_RXDATAV;
    }
}

static void uDebugDMACb(void *arg)
{
    uCfg[UART_DEBUG].txReady = UART_TX_READY;
    if (uCfg[UART_DEBUG].txCb)
    {
        uCfg[UART_DEBUG].txCb();
    }
}

/**@} uart */
