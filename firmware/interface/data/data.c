/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup data
 * @{
 */

/**************************************************************************//**
 * @file        data.c
 *
 * @brief       Provides an interface for the bootloader to send commands to
 *              and receive commands from the device loading the new
 *              application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "data.h"
#include "serial.h"
#include "timeout.h"

#define LENGTH_SIZE BL_SIZEOF(DataLength_t)

typedef enum
{
    GET_LENGTH = 0U,
    GET_DATA = 1U,
    NUM_STATES,
} data_States_t;

typedef struct
{
    BL_UINT32_T count;
} data_t;

BL_STATIC BL_BOOL_T dReady[NUM_STATES] = {BL_FALSE};
BL_STATIC BL_UINT32_T dCnt[NUM_STATES] = {0U};
BL_STATIC DataLength_t dLength = 0U;
BL_STATIC Timeout_Node_t dTimeout = {0U};
BL_STATIC BL_CONST Timeout_Cb_t dTimeoutCb[] =
{
    Serial_Flush,
    Data_Reset,
    BL_NULL,
};

BL_STATIC void length_Cb(BL_UINT32_T length);
BL_STATIC void data_Cb(BL_UINT32_T length);

BL_Err_t Data_LengthCbInit(void)
{
    BL_Err_t err = BL_ERR;

    if ((err = Timeout_Add(&dTimeout,
                           dTimeoutCb,
                           BL_SERIAL_TIMEOUT_MS)) == BL_OK)
    {
        err = Serial_RegisterCb(length_Cb);
    }

    return err;
}

BL_Err_t Data_LengthCbDeinit(void)
{
    BL_Err_t err = BL_ERR;

    if ((err = Timeout_Remove(&dTimeout)) == BL_OK)
    {
        err = Serial_DeregisterCb();
    }

    return err;
}

BL_Err_t Data_DataCbInit(void)
{
    BL_Err_t err = BL_ERR;

    if ((err = Timeout_Add(&dTimeout,
                           dTimeoutCb,
                           BL_SERIAL_TIMEOUT_MS)) == BL_OK)
    {
        err = Serial_RegisterCb(data_Cb);
    }

    return err;
}

BL_Err_t Data_DataCbDeinit(void)
{
    BL_Err_t err = BL_ERR;

    if ((err = Timeout_Remove(&dTimeout)) == BL_OK)
    {
        err = Serial_DeregisterCb();
    }

    return err;
}

BL_Err_t Data_GetLength(DataLength_t *length)
{
    BL_Err_t err = BL_ENODATA;
    BL_UINT8_T buf[LENGTH_SIZE] = {0U};

    if (dReady[GET_LENGTH] == BL_TRUE)
    {
        dReady[GET_LENGTH] = BL_FALSE;
        Serial_Receive(buf, LENGTH_SIZE);
        *length = (DataLength_t) (buf[0] << 24U |
                                  buf[1] << 16U |
                                  buf[2] << 8U |
                                  buf[3]);

        if (*length > BL_BUFFER_SIZE)
        {
            err = BL_ERR;
        }
        else
        {
            err = BL_OK;
        }
    }

    return err;
}

void Data_Reset(void)
{
    for (BL_UINT8_T dIdx = 0U; dIdx < NUM_STATES; dIdx++)
    {
        if (dCnt[dIdx] > 0U)
        {
            dCnt[dIdx] = 0U;
        }
    }
}

BL_Err_t Data_SetLength(DataLength_t length)
{
    BL_Err_t err = BL_EINVAL;

    if (length)
    {
        err = BL_OK;
        dLength = length;
    }

    return err;
}

BL_Err_t Data_ReceiveData(BL_UINT8_T *data)
{
    BL_Err_t err = BL_ENODATA;

    if (dReady[GET_DATA] == BL_TRUE)
    {
        dReady[GET_DATA] = BL_FALSE;
        Serial_Receive(data, dLength);
        dLength = 0U;
        err = BL_OK;
    }

    return err;
}

BL_STATIC void length_Cb(BL_UINT32_T length)
{
    Timeout_Kick(&dTimeout);
    dCnt[GET_LENGTH] += length;
    if (dCnt[GET_LENGTH] >= LENGTH_SIZE)
    {
        dCnt[GET_LENGTH] = 0U;
        dReady[GET_LENGTH] = BL_TRUE;
    }
}

BL_STATIC void data_Cb(BL_UINT32_T length)
{
    Timeout_Kick(&dTimeout);
    if (dLength != 0U)
    {
        dCnt[GET_DATA] += length;
        if (dCnt[GET_DATA] >= dLength)
        {
            dCnt[GET_DATA] = 0U;
            dReady[GET_DATA] = BL_TRUE;
        }
    }
}

/**@} data */
