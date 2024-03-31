/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

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
#include "writer.h"

#define DATA_INST_INIT(start, handle) { {start, handle}, D_BEGIN }
#define UPDATE_TASK_PERIOD_MS (5U)
#define ACK_READY(i) \
    i->response = TRANSMIT_READY; \
    i->flags.transmit = BL_TRUE;
#define NACK_READY(i, err) \
    i->response = TRANSMIT_ERROR; \
    i->flags.transmit = BL_TRUE; \
    i->state = COMMAND_HANDLE; \
    err = BL_ERR;

typedef enum
{
    COMMAND,
    DATA,
} update_State_e;

typedef enum
{
    COMMAND_HANDLE = 0U,
    GET_DATA_LENGTH,
    GET_DATA,
    DATA_HANDLE,
} states_e;

typedef BL_Err_t (*command_t)(void);
typedef BL_Err_t (*data_t)(BL_UINT8_T *buf, BL_UINT32_T size);
typedef void (*error_t)(BL_Err_t **err, BL_UINT8_T *count);
typedef struct
{
    command_t command;
    data_t data;
    error_t err;
} cb_t;

typedef struct
{
    cb_t cb;
    BL_BOOL_T data;
} cfg_t;

typedef struct
{
    cfg_t cfg;
    DataLength_t length;
    states_e state;
    struct
    {
        BL_Err_t *err;
        BL_UINT8_T count;
    } acceptable;
    Command_Transmit_e response;
    struct
    {
        uint8_t response : 1;
        uint8_t command : 1;
        uint8_t reset : 1;
        uint8_t init : 1;
        uint8_t transmit: 1;
        uint8_t : 3;
    } flags;
} inst_t;

BL_STATIC BL_CONST cfg_t lut[RECEIVE_NUM_COMMAND] =
{
    [RECEIVE_READY] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_ERROR] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_WRITE] =
    {
        { Writer_Start, Writer_WriteData, Writer_States }, BL_TRUE
    },
    [RECEIVE_READ] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_FINISH] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_RUN] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_VALIDATE] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_ERASE] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_LOCK] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_UNLOCK] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_RELEASE] =
    { 
        {Serial_Unlock, NULL, NULL}, BL_FALSE
    },
    [RECEIVE_RESET] =
    { 
        {NULL, NULL, NULL}, BL_FALSE
    },
};

BL_STATIC void update_Run(void);
BL_STATIC BL_Err_t response(inst_t *inst);
BL_STATIC BL_BOOL_T validate(inst_t *inst, BL_Err_t err);

BL_Err_t Update_Init(void)
{
    BL_Err_t err = BL_OK;
    BL_STATIC Schedule_Node_t node = {0};

    err = Schedule_Add(&node, UPDATE_TASK_PERIOD_MS, update_Run);

    return err;
}

BL_STATIC void update_Run(void)
{
    BL_STATIC inst_t inst = {0};
    Command_Receive_e cmd = RECEIVE_READY;
    BL_Err_t err = BL_OK;

    if (!inst.flags.init)
    {
        inst.flags.init = FLAG_SET;
        inst.flags.reset = FLAG_SET;
    }
    /* responding is top priority */
    if (inst.flags.response)
    {
        err = response(&inst);
        if (err == BL_OK || err == BL_ERR)
        {
            inst.flags.reset = FLAG_SET;
            inst.flags.response = FLAG_CLEAR;
        }
    }
    /* ensures that the next command will be ready */
    if (inst.flags.reset)
    {
        Command_Init();
        inst.flags.command = FLAG_SET;
        inst.flags.reset = FLAG_CLEAR;
    }
    /* transmits a response */
    if (inst.flags.transmit)
    {
        inst.flags.response = BL_FALSE;
        if (Command_Send(inst.response) == BL_OK)
        {
            inst.flags.transmit = BL_FALSE;
            inst.flags.response = BL_TRUE;
        }
    }
    /* handles new commands */
    if (inst.flags.command)
    {
        if (Command_Receive(&cmd) == BL_OK)
        {
            inst.cfg = lut[cmd];
            if (inst.cfg.cb.err)
            {
                inst.cfg.cb.err(&inst.acceptable.err,
                                &inst.acceptable.count);
            }
            Command_Deinit();
            inst.flags.command = FLAG_CLEAR;
            inst.flags.response = FLAG_SET;
        }
    }
}

BL_STATIC BL_Err_t response(inst_t *inst)
{
    BL_Err_t err = BL_OK;

    switch (inst->state)
    {
    case COMMAND_HANDLE:
        if (inst->cfg.cb.command)
        {
            err = inst->cfg.cb.command();
            if (err == BL_OK)
            {
                if (inst->cfg.data == BL_TRUE)
                {
                    inst->state = GET_DATA_LENGTH;
                    Data_LengthCbInit();
                }
                ACK_READY(inst);
                err = BL_EINPROGRESS;
            }
            else if (validate(inst, err) == BL_FALSE)
            {
                NACK_READY(inst, err);
            }
        }
        else
        {
            NACK_READY(inst, err);
        }
        break;
    case GET_DATA_LENGTH:
        if ((err = Data_GetLength(&inst->length)) == BL_OK)
        {
            Data_SetLength(inst->length);
            Data_LengthCbDeinit();
            Data_DataCbInit();
            ACK_READY(inst);
            inst->state = GET_DATA;
            err = BL_EINPROGRESS;
        }
        else if (err != BL_ENODATA)
        {
            Data_LengthCbDeinit();
            NACK_READY(inst, err);
        }
        break;
    case GET_DATA:
        if ((err = Data_ReceiveData(Buffer_Get())) == BL_OK)
        {
            Data_DataCbDeinit();
            inst->state = DATA_HANDLE;
            err = BL_EINPROGRESS;
        }
        else if (err != BL_ENODATA)
        {
            Data_DataCbDeinit();
            NACK_READY(inst, err);
        }
        break;
    case DATA_HANDLE:
        if (inst->cfg.cb.data)
        {
            err = inst->cfg.cb.data(Buffer_Get(), inst->length);
            if (err == BL_OK)
            {
                inst->length = 0U;
                inst->state = COMMAND_HANDLE;
                MEMSET(Buffer_Get(), 0U, BL_BUFFER_SIZE);
                ACK_READY(inst);
            }
            else if (validate(inst, err) == BL_FALSE)
            {
                NACK_READY(inst, err);
            }
        }
        else
        {
            NACK_READY(inst, err);
        }
        break;
    default:
        break;
    }

    return err;
}

BL_STATIC BL_BOOL_T validate(inst_t *inst, BL_Err_t err)
{
    BL_BOOL_T ret = err == BL_OK ? BL_TRUE : BL_FALSE;
    if (ret == BL_FALSE && inst->acceptable.err)
    {
        for (BL_UINT8_T i = 0; i < inst->acceptable.count; i++)
        {
            if (err == inst->acceptable.err[i])
            {
                ret = BL_TRUE;
                break;
            }
        }
    }
    return ret;
}
