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
#include "updater.h"

#define DATA_INST_INIT(start, handle) { {start, handle}, D_BEGIN }
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
    D_LENGTH,
    D_DATA,
    D_HANDLE,
} data_States_e;

typedef BL_Err_t (*Data_Start_t)(void);
typedef BL_Err_t (*Data_Handle_t)(BL_UINT8_T *buf, BL_UINT32_T size);
typedef struct
{
    Data_Start_t start;
    Data_Handle_t handle;
} Data_Cb_t;

typedef struct
{
    Data_Cb_t cb;
    data_States_e state;
    DataLength_t length;
} Data_Inst_t;

BL_STATIC BL_CONST Data_Cb_t inst_LUT[RECEIVE_NUM_COMMAND] =
{
    [RECEIVE_READY] = {NULL, NULL},
    [RECEIVE_ERROR] = {NULL, NULL},
    [RECEIVE_WRITE] = {Updater_Start, Updater_Handle},
    [RECEIVE_RUN] = {NULL, NULL},
    [RECEIVE_VALIDATE] = {NULL, NULL},
    [RECEIVE_ERASE] = {NULL, NULL},
    [RECEIVE_LOCK] = {NULL, NULL},
    [RECEIVE_UNLOCK] = {NULL, NULL},
    [RECEIVE_RELEASE] = {NULL, NULL},
    [RECEIVE_RESET] = {NULL, NULL},
};


BL_STATIC void update_Run(void);
BL_STATIC update_State_e command_Handler(Command_Receive_e command);
BL_STATIC BL_Err_t data_Handler(Data_Inst_t *inst);

BL_Err_t Update_Init(void)
{
    BL_Err_t err = BL_OK;
    BL_STATIC Schedule_Node_t node = {0};

    err = Schedule_Add(&node, UPDATE_TASK_PERIOD_MS, update_Run);

    return err;
}

BL_STATIC void update_Run(void)
{
    BL_STATIC Command_Receive_e cmd = RECEIVE_READY;
    BL_STATIC Data_Inst_t *inst = NULL;
    update_State_e state = COMMAND;
    BL_Err_t err = BL_OK;

    switch (state)
    {
    case COMMAND:
        Command_Init();
        if (Command_Receive(&cmd) == BL_OK)
        {
            state = command_Handler(cmd);
        }
        else
        {
            break;
        }
    case DATA:
        if ((err = data_Handler(inst)) == BL_OK ||
                err == BL_ERR)
        {
            state = COMMAND;
        }
        break;
    }
}

/* TODO: cases will set a data callback, and the data callback will be handled until it is told to change data state */
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

BL_STATIC BL_Err_t data_Handler(Data_Inst_t *inst)
{
    BL_Err_t err = BL_OK;

    if (inst->cb.handle && inst->cb.start)
    {
        switch (inst->state)
        {
        case D_BEGIN:
            if (inst->cb.start)
            {
                if ((err = inst->cb.start()) == BL_OK)
                {
                    inst->state = D_LENGTH;
                    Data_LengthCbInit();
                    ACK_READY();
                }
                else 
                {
                    break;
                }
            }
        case D_LENGTH:
            if ((err = Data_GetLength(&inst->length)) == BL_OK)
            {
                Data_SetLength(inst->length);
                Data_LengthCbDeinit();
                Data_DataCbInit();
                ACK_READY();
                inst->state = D_DATA;
            }
            else if (err == BL_ENODATA)
            {
                break;
            }
            else
            {
                NACK_READY();
                inst->state = D_BEGIN;
                break;
            }
        case D_DATA:
            if ((err = Data_ReceiveData(Buffer_Get())) == BL_OK)
            {
                Data_DataCbDeinit();
                inst->state = D_HANDLE;
            }
            else if (err == BL_ENODATA)
            {
                break;
            }
            else
            {
                inst->state = D_BEGIN;
                break;
            }
        case D_HANDLE:
            inst->state = D_BEGIN;
            if (inst->cb.handle)
            {
                if ((err = inst->cb.handle(Buffer_Get(),
                                           inst->length)) == BL_OK)
                {
                    inst->length = 0U;
                    inst->state = D_BEGIN;
                    MEMSET(Buffer_Get(), 0U, BL_BUFFER_SIZE);
                    //TODO: ACK here
                }
                else if (err != BL_EALREADY)
                {
                    inst->state = D_BEGIN;
                    //TODO: NACK here
                }
            }
            break;
        default:
            break;
        }
    }

    return err;
}
