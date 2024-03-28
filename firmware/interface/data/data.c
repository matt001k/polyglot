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
#include "helper.h"

#define LENGTH_SIZE BL_SIZEOF(DataLength_t)

typedef enum
{
    GET_LENGTH = 0U,
    GET_DATA = 1U,
    NUM_STATES,
} data_States_t;

BL_STATIC struct
{
    BL_BOOL_T ready[NUM_STATES];
    BL_UINT32_T count[NUM_STATES];
    DataLength_t length;
    Timeout_Node_t timeout;
} data = {0};
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

    if ((err = Timeout_Add(&data.timeout,
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

    if ((err = Timeout_Remove(&data.timeout)) == BL_OK)
    {
        err = Serial_DeregisterCb();
    }

    return err;
}

BL_Err_t Data_DataCbInit(void)
{
    BL_Err_t err = BL_ERR;

    if ((err = Timeout_Add(&data.timeout,
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

    if ((err = Timeout_Remove(&data.timeout)) == BL_OK)
    {
        err = Serial_DeregisterCb();
    }

    return err;
}

BL_Err_t Data_GetLength(DataLength_t *length)
{
    BL_Err_t err = BL_ENODATA;
    BL_UINT8_T buf[LENGTH_SIZE] = {0U};

    if (data.ready[GET_LENGTH] == BL_TRUE)
    {
        data.ready[GET_LENGTH] = BL_FALSE;
        err = Serial_Receive(buf, LENGTH_SIZE);
        UINT8_UINT32(length, buf);

        if (*length > BL_BUFFER_SIZE)
        {
            err = BL_ERR;
        }
    }
    else
    {
        err = BL_ENODATA;
    }

    return err;
}

void Data_Reset(void)
{
    for (BL_UINT8_T dIdx = 0U; dIdx < NUM_STATES; dIdx++)
    {
        if (data.count[dIdx] > 0U)
        {
            data.count[dIdx] = 0U;
        }
    }
}

BL_Err_t Data_SetLength(DataLength_t length)
{
    BL_Err_t err = BL_EINVAL;

    if (length)
    {
        err = BL_OK;
        data.length = length;
    }

    return err;
}

BL_Err_t Data_ReceiveData(BL_UINT8_T *buf)
{
    BL_Err_t err = BL_ERR;

    if (data.ready[GET_DATA] == BL_TRUE)
    {
        data.ready[GET_DATA] = BL_FALSE;
        err = Serial_Receive(buf, data.length);
        data.length = 0U;
    }
    else
    {
        err = BL_ENODATA;
    }

    return err;
}

BL_STATIC void length_Cb(BL_UINT32_T length)
{
    Timeout_Kick(&data.timeout);
    data.count[GET_LENGTH] += length;
    if (data.count[GET_LENGTH] >= LENGTH_SIZE)
    {
        data.count[GET_LENGTH] = 0U;
        data.ready[GET_LENGTH] = BL_TRUE;
    }
}

BL_STATIC void data_Cb(BL_UINT32_T length)
{
    Timeout_Kick(&data.timeout);
    if (data.length != 0U)
    {
        data.count[GET_DATA] += length;
        if (data.count[GET_DATA] >= data.length)
        {
            data.count[GET_DATA] = 0U;
            data.ready[GET_DATA] = BL_TRUE;
        }
    }
}

/**@} data */
