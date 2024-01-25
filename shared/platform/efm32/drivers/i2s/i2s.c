/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup i2s
 * @{
 */

/**
 * @file        i2s.c
 *
 * @brief       Provides the API for I2S peripheral in slave mode
 *
 * @author      Matthew Krause
 *
 * @date        2022-12-02
 */
/*****************************************************************************/
#include "i2s.h"
#include "clock.h"
#include "dma.h"
#include "atomic.h"
#include <stddef.h>

#define I2S_FLAG_CLEAR (0U)
#define I2S_FLAG_SET (1U)
#define I2S_BUF_SIZE (256U)
#define I2S_LEFT (0U)
#define I2S_RIGHT (~I2S_LEFT)
#define I2S_BAUD_CALC(baud, var)                        \
do                                                      \
{                                                       \
    var = Clock_GetFrequency(0U) / (2UL * baud) - 1U;   \
    var = var << 8;                                     \
} while (0U)
typedef uint32_t i2s_Location_t;
typedef struct
{
    uint16_t rx[2U];
    uint16_t tx[2U];
} i2s_Data_t;

typedef struct
{
    USART_TypeDef *i2s;
    uint32_t baud;
    i2s_Location_t clk;
    i2s_Location_t miso;
    i2s_Location_t mosi;
    i2s_Location_t cs;
    I2S_TxCallback_t txCb;
    I2S_RxCallback_t rxCb;
    uint8_t irq;
    i2s_Data_t data;
    uint8_t state;
} i2s_t;

void USART3_IRQHandler(void *arg);

static i2s_t i2s[NUM_I2S] =
{
    [I2S_CODEC] =
    {
        USART3,
        1540000U,
        USART_ROUTELOC0_CLKLOC_LOC0,
        USART_ROUTELOC0_RXLOC_LOC0,
        USART_ROUTELOC0_TXLOC_LOC0,
        USART_ROUTELOC0_CSLOC_LOC0,
        NULL,
        NULL,
        7U,
        {{0U}},
        I2S_LEFT,
    }
};

void I2S_Init(void)
{
    uint32_t clock = CMU_HFPERCLKEN0_USART0;
    IRQn_Type irq = 0U;
    ENTER_CRITICAL();
    for (uint8_t i = 0U; i < NUM_I2S; i++)
    {
        /* Configure the required clock */
        if(i2s[i].i2s == USART0)
        {
            clock = CMU_HFPERCLKEN0_USART0;
            irq = USART0_IRQn;
        }
        else if(i2s[i].i2s == USART1)
        {
            clock = CMU_HFPERCLKEN0_USART1;
            irq = USART1_IRQn;
        }
        else if(i2s[i].i2s == USART2)
        {
            clock = CMU_HFPERCLKEN0_USART2;
            irq = USART2_IRQn;
        }
        else
        {
            clock = CMU_HFPERCLKEN0_USART3;
            irq = USART3_IRQn;
        }
        CMU->HFPERCLKEN0 |= clock;
        /* Configure peripheral */
        i2s[i].i2s->CTRL = USART_CTRL_SMSDELAY | USART_CTRL_SSSEARLY |
                              USART_CTRL_SYNC | USART_CTRL_MSBF;
        i2s[i].i2s->FRAME = USART_FRAME_DATABITS_SIXTEEN |
                               USART_FRAME_PARITY_NONE |
                               USART_FRAME_STOPBITS_DEFAULT;
        I2S_BAUD_CALC(i2s[i].baud, clock);
        i2s[i].i2s->CLKDIV = clock;
        i2s[i].i2s->TIMING = _USART_TIMING_RESETVALUE;
        /* Route pins required */
        i2s[i].i2s->ROUTELOC0 = i2s[i].mosi | i2s[i].miso |
                                   i2s[i].clk | i2s[i].cs;
        i2s[i].i2s->ROUTEPEN = USART_ROUTEPEN_RXPEN |
                               UART_ROUTEPEN_TXPEN |
                               UART_ROUTEPEN_CLKPEN |
                               UART_ROUTEPEN_CSPEN;
        /* I2S Configuration */
        i2s[i].i2s->I2SCTRL = USART_I2SCTRL_FORMAT_W16D16 |
                              USART_I2SCTRL_DELAY |
                              USART_I2SCTRL_EN;
        /* Place in Slave Mode */
        i2s[i].i2s->CMD = USART_CMD_MASTEREN;
        while (!(i2s[i].i2s->STATUS & USART_STATUS_MASTER)) {};
        /* Enable Interrupts */
        i2s[i].i2s->IEN = USART_IEN_RXDATAV | USART_IEN_TXC;
        NVIC_ClearPendingIRQ(irq);
        NVIC_SetPriority(irq, i2s[i].irq);
        NVIC_EnableIRQ(irq);
        /* Enable RX/TX channel and begin transmissions */
        i2s[i].i2s->CMD = USART_CMD_RXEN | USART_CMD_TXEN;
        while (!(i2s[i].i2s->STATUS & USART_STATUS_RXENS)) {};
        i2s[i].state = I2S_LEFT; 
        i2s[i].i2s->TXDOUBLE = 0U;
    }
    EXIT_CRITICAL();
}

void I2S_Deinit(void)
{
    uint32_t clock = CMU_HFPERCLKEN0_USART0;
    IRQn_Type irq = 0U;
    ENTER_CRITICAL();
    for (uint8_t i = 0U; i < NUM_I2S; i--)
    {
        /* Clear register values */
        i2s[i].i2s->CTRL = 0U;
        i2s[i].i2s->FRAME = 0U;
        i2s[i].i2s->TRIGCTRL = 0U;
        i2s[i].i2s->CLKDIV = 0U;
        i2s[i].i2s->IEN = 0U;
        i2s[i].i2s->IFC = 0U;
        i2s[i].i2s->TIMING = 0U;
        i2s[i].i2s->ROUTEPEN = 0U;
        i2s[i].i2s->ROUTELOC0 = 0U;
        i2s[i].i2s->ROUTELOC1 = 0U;
        i2s[i].i2s->I2SCTRL = 0U;
        /* Disable clock for the peripheral */
        if(i2s[i].i2s == USART0)
        {
            clock = CMU_HFPERCLKEN0_USART0;
            irq = USART0_IRQn;
        }
        else if(i2s[i].i2s == USART1)
        {
            clock = CMU_HFPERCLKEN0_USART1;
            irq = USART1_IRQn;
        }
        else if(i2s[i].i2s == USART2)
        {
            clock = CMU_HFPERCLKEN0_USART2;
            irq = USART2_IRQn;
        }
        else
        {
            clock = CMU_HFPERCLKEN0_USART3;
            irq = USART3_IRQn;
        }
        CMU->HFPERCLKEN0 &= ~(clock);
        /* Disable Interrupts */
        NVIC_DisableIRQ(irq);
        NVIC_ClearPendingIRQ(irq);
    }
    EXIT_CRITICAL();
}

void I2S_Transmit(I2S_Type_t type, uint16_t left, uint16_t right)
{
    if (type < NUM_I2S)
    {
        /* Begin transaction and clear ready flag */
        ENTER_CRITICAL();
        i2s[type].data.tx[I2S_LEFT] = left;
        i2s[type].data.tx[I2S_RIGHT] = right;
        EXIT_CRITICAL();
    }
}

void I2S_Receive(I2S_Type_t type, uint16_t *left, uint16_t *right)
{
    if (type < NUM_I2S && left && right)
    {
        /* Clear ready flags */
        ENTER_CRITICAL();
        *left = i2s[type].data.rx[I2S_LEFT];
        *right = i2s[type].data.rx[I2S_RIGHT];
        EXIT_CRITICAL();
    }
}

void I2S_RegisterTxCallback(I2S_Type_t type, I2S_TxCallback_t cb)
{
    if (type < NUM_I2S && cb)
    {
        i2s[type].txCb = cb;
    }
}

void I2S_RegisterRxCallback(I2S_Type_t type, I2S_RxCallback_t cb)
{
    if (type < NUM_I2S && cb)
    {
        i2s[type].rxCb = cb;
    }
}

void USART3_IRQHandler(void *arg)
{
    if (i2s[I2S_CODEC].i2s->IF & USART_IF_RXDATAV)
    {
        if (i2s[I2S_CODEC].state == I2S_LEFT)
        {
            i2s[I2S_CODEC].data.rx[I2S_LEFT] =
                (uint16_t) i2s[I2S_CODEC].i2s->RXDOUBLE;
        }
        else
        {
            i2s[I2S_CODEC].data.rx[I2S_RIGHT] =
                (uint16_t) i2s[I2S_CODEC].i2s->RXDOUBLE;
            if (i2s[I2S_CODEC].rxCb)
            {
                i2s[I2S_CODEC].rxCb();
            }
        }
    }
    if (i2s[I2S_CODEC].i2s->IF & USART_IF_TXC)
    {
        if (i2s[I2S_CODEC].state == I2S_LEFT)
        {
            i2s[I2S_CODEC].i2s->TXDOUBLE = i2s[I2S_CODEC].data.tx[I2S_LEFT];
        }
        else
        {
            i2s[I2S_CODEC].i2s->TXDOUBLE = i2s[I2S_CODEC].data.tx[I2S_RIGHT];
            if (i2s[I2S_CODEC].txCb)
            {
                i2s[I2S_CODEC].txCb();
            }
        }
        i2s[I2S_CODEC].i2s->IFC = USART_IFC_TXC;
    }
    i2s[I2S_CODEC].state = ~i2s[I2S_CODEC].state;
}

/**@} i2s */

