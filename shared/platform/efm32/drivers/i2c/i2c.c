
/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup i2c
 * @{
 */

/**
 * @file        i2c.c
 *
 * @brief       Provides the API for I2C
 *
 * @author      Matthew Krause
 *
 * @date        2022-11-09
 */

/*****************************************************************************/
#include "i2c.h"
#include "atomic.h"
#include "clock.h"

#define I2C_TRANSMIT (0U)
#define I2C_RECEIVE (1U)
#define I2C_TX_BUFFER_FULL(t) \
(!(i2c[t].i2c->IF & I2C_IF_TXBL))
#define I2C_TRANSFER_ONGOING(t) \
(!(i2c[t].i2c->IF & (I2C_IF_NACK | I2C_IF_ACK | \
                     I2C_IF_BUSERR | I2C_IF_ARBLOST)))
#define I2C_ACK_CLEAR(t) \
(i2c[t].i2c->IFC = I2C_IFC_ACK)
#define I2C_ERRORS(t) \
    (i2c[t].i2c->IF & (I2C_IF_NACK | I2C_IF_BUSERR | I2C_IF_ARBLOST))
#define I2C_BAUD_CALC(baud, var, N)                                        \
do                                                                         \
{                                                                          \
    var = (((Clock_GetFrequency(0U) / baud) - 8U) / i2c_NSum_LUT[N]) - 1U; \
} while (0U)
#define I2C_IF_MASK 0x001FFFFFUL

#define I2C_CODEC_BAUD (100000U)

typedef uint32_t i2c_Location_t;

typedef enum
{
    CLHR_DEFAULT,
    CLHR_ASYMMETRIC,
    CLHR_FAST,
} i2c_CLHR_t;

typedef struct
{
    I2C_TypeDef *i2c;
    uint32_t baud;
    i2c_Location_t scl;
    i2c_Location_t sda;
} i2c_t;

static const uint8_t i2c_NSum_LUT[] =
    {
        [CLHR_DEFAULT] = 4U + 4U,
        [CLHR_ASYMMETRIC] = 6U + 3U,
        [CLHR_FAST] = 11U + 6U,
};

static i2c_t i2c[NUM_I2C] =
    {
        [I2C_CODEC] =
            {
                I2C0,
                I2C_CODEC_BAUD,
                I2C_ROUTELOC0_SCLLOC_LOC1,
                I2C_ROUTELOC0_SDALOC_LOC1,
            },
};

static inline uint8_t i2c_AddressSend(I2C_Type_t type,
                                      uint8_t address,
                                      uint8_t direction);

void I2C_Init(void)
{
    uint32_t iClockSelect = CMU_HFPERCLKEN0_I2C0;
    for (uint8_t iIdx = 0U; iIdx < NUM_I2C; iIdx++)
    {
        ENTER_CRITICAL();
        /* Clock determination */
        if (i2c[iIdx].i2c == I2C0)
        {
            iClockSelect = CMU_HFPERCLKEN0_I2C0;
        }
        else
        {
            iClockSelect = CMU_HFPERCLKEN0_I2C1;
        }
        CMU->HFPERCLKEN0 |= iClockSelect;
        /* Route I2C lines and configure peripheral, also clear busy status */
        i2c[iIdx].i2c->ROUTEPEN = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;
        i2c[iIdx].i2c->ROUTELOC0 = i2c[iIdx].scl | i2c[iIdx].sda;
        I2C_BAUD_CALC(i2c[iIdx].baud, i2c[iIdx].i2c->CLKDIV, CLHR_DEFAULT);
        i2c[iIdx].i2c->CTRL = I2C_CTRL_CLHR_DEFAULT | I2C_CTRL_EN;
        i2c[iIdx].i2c->CMD = I2C_CMD_ABORT;
        EXIT_CRITICAL();
    }
}

void I2C_Deinit(void)
{
    uint32_t iClockSelect = CMU_HFPERCLKEN0_I2C0;
    for (uint8_t iIdx = 0U; iIdx < NUM_I2C; iIdx++)
    {
        ENTER_CRITICAL();
        i2c[iIdx].i2c->ROUTEPEN = 0U;
        i2c[iIdx].i2c->ROUTELOC0 = 0U;
        i2c[iIdx].i2c->CLKDIV = 0U;
        i2c[iIdx].i2c->CTRL = 0U;
        if (i2c[iIdx].i2c == I2C0)
        {
            iClockSelect = CMU_HFPERCLKEN0_I2C0;
        }
        else
        {
            iClockSelect = CMU_HFPERCLKEN0_I2C1;
        }
        CMU->HFPERCLKEN0 &= ~(iClockSelect);
        EXIT_CRITICAL();
    }
}

void I2C_Transmit(I2C_Type_t type,
                  uint8_t address,
                  uint8_t *data,
                  uint32_t size,
                  uint8_t stop)
{

    if (!(i2c[type].i2c->IF & (I2C_IF_BUSERR | I2C_IF_ARBLOST)))
    {
        if (!i2c_AddressSend(type, address, I2C_TRANSMIT))
        {
            /* Send I2C data */
            while (size--)
            {
                i2c[type].i2c->TXDATA = *data++;
                while (I2C_TRANSFER_ONGOING(type)) {};
                if (I2C_ERRORS(type))
                {
                    i2c[type].i2c->CMD = I2C_CMD_STOP;
                    break;
                }
                I2C_ACK_CLEAR(type);
            }
            if (stop == I2C_STOP)
            {
                i2c[type].i2c->CMD = I2C_CMD_STOP;
            }
        }
    }
    if (I2C_ERRORS(type))
    {
        i2c[type].i2c->IFC = I2C_IFC_BUSERR | I2C_IFC_ARBLOST | I2C_IFC_NACK;
    }
}

void I2C_Receive(I2C_Type_t type,
                 uint8_t address,
                 uint8_t *data,
                 uint32_t size,
                 uint8_t stop)
{
    if (!(i2c[type].i2c->IF & (I2C_IF_BUSERR | I2C_IF_ARBLOST)))
    {
        if (!i2c_AddressSend(type, address, I2C_RECEIVE))
        {
            /* Receive I2C data */
            while (size--)
            {
                while (!(i2c[type].i2c->IF & I2C_IF_RXDATAV)) {};
                *data++ = (uint8_t) i2c[type].i2c->RXDATA;
                if (I2C_ERRORS(type))
                {
                    i2c[type].i2c->CMD = I2C_CMD_STOP;
                    break;
                }
                if (size == 0U)
                {
                    i2c[type].i2c->CMD = I2C_CMD_NACK;
                }
                else
                {
                    i2c[type].i2c->CMD = I2C_CMD_ACK;
                }
            }
            if (stop == I2C_STOP)
            {
                i2c[type].i2c->CMD = I2C_CMD_STOP;
            }
        }
    }
    if (I2C_ERRORS(type))
    {
        i2c[type].i2c->IFC = I2C_IFC_BUSERR | I2C_IFC_ARBLOST | I2C_IFC_NACK;
    }
}

void I2C_Flush(I2C_Type_t type)
{
    /* Flush TX buffer and read until RX buffer is flushed */
    i2c[type].i2c->CMD = I2C_CMD_CLEARPC | I2C_CMD_CLEARTX;
    while (i2c[type].i2c->STATUS & I2C_STATUS_RXDATAV)
    {
        i2c[type].i2c->RXDATA;
    }
}

static inline uint8_t i2c_AddressSend(I2C_Type_t type,
                                      uint8_t address,
                                      uint8_t direction)
{
    uint8_t ret = 0U;
    /* Flush buffers and clear interrupts */
    I2C_Flush(type);
    i2c[type].i2c->IFC = I2C_IF_MASK;
    while (I2C_TX_BUFFER_FULL(type)) {};
    /* Transmit the address, determine if transaction is ongoing */
    if (i2c[type].i2c->STATE & I2C_STATE_BUSHOLD)
    {
        i2c[type].i2c->CMD = I2C_CMD_START;
        i2c[type].i2c->TXDATA = address | direction;
    }
    else
    {
        i2c[type].i2c->TXDATA = address | direction;
        i2c[type].i2c->CMD = I2C_CMD_START;
    }
    while (I2C_TRANSFER_ONGOING(type)) {};
    /* Error determination... if not ack received */
    if (I2C_ERRORS(type))
    {
        ret = 1U;
        i2c[type].i2c->CMD = I2C_CMD_STOP;
    }
    else
    {
        I2C_ACK_CLEAR(type); 
    }
    return ret;
}