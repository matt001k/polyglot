/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup update
 * @{
 */

/**************************************************************************//**
 * @file        update.c
 *
 * @brief       Task to update the application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-30
 *****************************************************************************/
#include "update.h"
#include "data.h"
#include "command.h"
#include "schedule.h"
#include "serial.h"
#include "loader.h"
#include "helper.h"
#include "jump.h"
#include "buffer.h"
#include "validator.h"

#define UPDATE_TASK_PERIOD_MS (5U)
#define ACK_READY() Command_Send(TRANSMIT_READY)
#define NACK_READY() Command_Send(TRANSMIT_ERROR)

typedef enum
{
    COMMAND = 0U,
    DATA,
} update_State_e;

typedef enum
{
    D_BEGIN = 0U,
    D_INIT,
    D_LENGTH,
    D_DATA,
} data_State_e;

BL_STATIC Schedule_Node_t uNode = {0U};

BL_STATIC void update_Run(void);
BL_STATIC update_State_e command_Handler(Command_Receive_e command);
BL_STATIC update_State_e data_Handler(Command_Receive_e command);

BL_Err_t Update_Init(void)
{
    BL_Err_t err = BL_OK;

    err = Schedule_Add(&uNode, UPDATE_TASK_PERIOD_MS, update_Run);

    return err;
}

BL_STATIC void update_Run(void)
{
    BL_STATIC update_State_e state = COMMAND;
    BL_STATIC Command_Receive_e cmd = RECEIVE_READY;

    switch (state)
    {
    case COMMAND:
        Command_Init();
        if (Command_Receive(&cmd) == BL_OK)
        {
            state = command_Handler(cmd);
        }
        break;
    case DATA:
        state = data_Handler(cmd);
        break;
    default:
        break;
    }
}

BL_STATIC update_State_e command_Handler(Command_Receive_e cmd)
{
    update_State_e state = COMMAND;

    switch (cmd)
    {
    case RECEIVE_WRITE:
        /* Intentional Fallthrough */
    case RECEIVE_VALIDATE:
        Command_Deinit();
        state = DATA;
        break;
    case RECEIVE_RUN:
        if(Validator_Run(Buffer_Get(), BL_BUFFER_SIZE) == BL_OK)
        {
            ACK_READY();
            Jump_ToApp();
        }
        else
        {
            NACK_READY();
        }
        break;
    case RECEIVE_ERASE:
        ACK_READY();
        break;
    case RECEIVE_LOCK:
        break;
    case RECEIVE_UNLOCK:
        break;
    case RECEIVE_RELEASE:
        Serial_Unlock();
        break;
    default:
        break;
    }

    return state;
}

BL_STATIC update_State_e data_Handler(Command_Receive_e command)
{
    update_State_e uState = DATA;
    BL_STATIC data_State_e dState = D_BEGIN;
    BL_STATIC DataLength_t length = 0U;
    BL_STATIC BL_BOOL_T vInit = BL_FALSE;
    BL_STATIC BL_BOOL_T wOngoing = BL_FALSE;
    BL_STATIC BL_BOOL_T vOngoing = BL_FALSE;
    BL_Err_t vErr = BL_ERR;

    if (command == RECEIVE_VALIDATE)
    {
        if (vInit == BL_FALSE)
        {
            if ((vErr = Loader_WriteSecret(Buffer_Get(), BL_BUFFER_SIZE)) ==
                BL_OK ||
                vErr == BL_ERR)
            {
                Loader_Reset();

                if (vErr == BL_OK)
                {
                    vInit = BL_TRUE;
                }
                else if (vErr == BL_ERR)
                {
                    dState = D_BEGIN;
                    uState = COMMAND;
                    NACK_READY();
                }
            }
        }
        if (vOngoing != BL_TRUE && vInit == BL_TRUE)
        {
            vErr = Loader_Validate(Buffer_Get(), BL_BUFFER_SIZE);
        }
        if ((vErr == BL_OK || vErr == BL_ERR || vOngoing == BL_TRUE) &&
            vInit == BL_TRUE)
        {
            if (vErr == BL_OK || vErr == BL_ERR)
            {
                Loader_Reset();
            }
            if (vErr == BL_OK || vOngoing == BL_TRUE)
            {
                vOngoing = BL_TRUE;
                vErr = Loader_UpdateRevisions(Buffer_Get(), BL_BUFFER_SIZE);
            }
            if (vErr == BL_OK || vErr == BL_ERR || vOngoing == BL_FALSE)
            {
                Loader_Reset();
                MEMSET(Buffer_Get(), 0U, BL_BUFFER_SIZE);
                dState = D_BEGIN;
                uState = COMMAND;
                vInit = BL_FALSE;
                vOngoing = BL_FALSE;
                Command_Init();
                if (vErr == BL_OK)
                {
                    ACK_READY();
                }
                else
                {
                    NACK_READY();
                }
            }
        }
    }
    else
    {
        switch (dState)
        {
        case D_BEGIN:
            if (Loader_Init(Buffer_Get(), BL_BUFFER_SIZE) == BL_OK)
            {
                dState = D_INIT;
            }
            break;
        case D_INIT:
            Data_LengthCbInit();
            ACK_READY();
            dState = D_LENGTH;
            break;
        case D_LENGTH:
            if (Data_GetLength(&length) == BL_OK)
            {
                Data_SetLength(length);
                Data_LengthCbDeinit();
                Data_DataCbInit();
                ACK_READY();
                dState = D_DATA;
            }
            break;
        case D_DATA:
            if (Data_ReceiveData(Buffer_Get()) == BL_OK || wOngoing == BL_TRUE)
            {
                if (wOngoing == BL_FALSE)
                {
                    wOngoing = BL_TRUE;
                    Data_DataCbDeinit();
                }
                if (Loader_Write(Buffer_Get(), length) == BL_OK)
                {
                    wOngoing = BL_FALSE;
                    length = 0U;
                    MEMSET(Buffer_Get(), 0U, BL_BUFFER_SIZE);
                    dState = D_INIT;
                    uState = COMMAND;
                    Command_Init();
                    ACK_READY();
                }
            }
        default:
            break;
        }
    }

    return uState;
}


/**@} update */
