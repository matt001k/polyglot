/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup command
 * @{
 */

/**************************************************************************//**
 * @file        command.c
 *
 * @brief       Provides an interface for the bootloader to send commands to
 *              and receive commands from the device loading the new
 *              application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "command.h"
#include "dict.h"
#include "serial.h"
#include "helper.h"
#include "timeout.h"

#define COMMAND_SIZE BL_SIZEOF(Dict_Item_t)

BL_STATIC struct
{
    BL_BOOL_T ready;
    BL_BOOL_T cb;
    BL_UINT8_T count;
    Timeout_Node_t timeout;
} command = {0};
BL_STATIC BL_CONST Timeout_Cb_t cTimeoutCb[] =
{
    Serial_Flush,
    Command_Reset,
    BL_NULL,
};

BL_STATIC BL_CONST Dict_Item_t tList[TRANSMIT_NUM_COMMAND] =
{
    [TRANSMIT_READY] = BL_READY,
    [TRANSMIT_ERROR] = BL_ERROR,
};

BL_STATIC BL_CONST Dict_Item_t rList[RECEIVE_NUM_COMMAND] =
{
    [RECEIVE_READY] = BL_READY,
    [RECEIVE_ERROR] = BL_ERROR,
    [RECEIVE_WRITE] = BL_WRITE,
    [RECEIVE_RUN] = BL_RUN,
    [RECEIVE_VALIDATE] = BL_VALIDATE,
    [RECEIVE_ERASE] = BL_ERASE,
    [RECEIVE_LOCK] = BL_LOCK,
    [RECEIVE_UNLOCK] = BL_UNLOCK,
    [RECEIVE_RELEASE] = BL_RELEASE_PORT,
    [RECEIVE_RESET] = BL_RESET,
};

BL_STATIC void command_Cb(BL_UINT32_T length);

BL_Err_t Command_Init(void)
{
    BL_Err_t err = BL_OK;

    if (command.cb == BL_FALSE)
    {
        command.cb = BL_TRUE;
        err = Timeout_Add(&command.timeout, cTimeoutCb, BL_SERIAL_TIMEOUT_MS);
        if (err == BL_OK)
        {
            err = Serial_RegisterCb(command_Cb);
        }
    }

    return err; 
}

BL_Err_t Command_Deinit(void)
{
    BL_Err_t err = BL_OK;

    if (command.cb == BL_TRUE)
    {
        command.cb = BL_FALSE;
        err = Timeout_Remove(&command.timeout);
        if ( err == BL_OK)
        {
            err = Serial_DeregisterCb();
        }
    }

    return err;
}

void Command_Reset(void)
{
    if (command.count > 0U)
    {
        command.count = 0U;
    }
}

BL_Err_t Command_Send(Command_Transmit_e cmd)
{
    BL_Err_t err = BL_EINVAL;
    uint8_t buf[COMMAND_SIZE] = {0U};

    if (cmd < TRANSMIT_NUM_COMMAND)
    {
        UINT32_UINT8(buf, tList[cmd]);
        err = Serial_Transmit(buf, COMMAND_SIZE);
    }

    return err;
}

BL_Err_t Command_Receive(Command_Receive_e *cmd)
{
    BL_Err_t err = BL_ENODATA;
    BL_UINT8_T buf[COMMAND_SIZE] = {0U};
    Dict_Item_t item = 0U;

    if (command.ready == BL_TRUE)
    {
        command.ready = BL_FALSE;
        Serial_Receive(buf, COMMAND_SIZE);
        UINT8_UINT32(&item, buf);
        err = BL_ENOMSG;

        for (uint8_t cIdx = 0U; cIdx < RECEIVE_NUM_COMMAND; cIdx++)
        {
            if (rList[cIdx] == item)
            {
                *cmd = (Command_Receive_e) cIdx;
                err = BL_OK;
                break;
            }
        }
    }

    return err;
}

BL_STATIC void command_Cb(BL_UINT32_T length)
{
    Timeout_Kick(&command.timeout);
    command.count += length;

    if (command.count >= COMMAND_SIZE)
    {
        command.ready = BL_TRUE;
        command.count = 0U;
    }
}

/**@} command */
