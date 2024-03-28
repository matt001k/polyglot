/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**************************************************************************//**
 * @file        writer.c
 *
 * @brief       Provides an interface to write/read from partitions.
 *              This API has the ability to update and interact with the
 *              partition table API.
 *
 * @author      Matthew Krause
 *
 * @date        2024-03-10
 *****************************************************************************/
#include "writer.h"
#include "config.h"
#include "table.h"
#include "nvm.h"
#include "helper.h"

BL_STATIC struct writer_s
{
    BL_UINT32_T size;
    NVM_Node_t node;
    struct flags_s
    {
        BL_UINT8_T start : 1;
        BL_UINT8_T erase : 1;
        BL_UINT8_T : 6;
    } flags;
} writer = {0};

BL_STATIC BL_CONST BL_Err_t acceptable[] =
{
    BL_EALREADY,
    BL_EINPROGRESS,
};

BL_Err_t Writer_Start(void)
{
    BL_Err_t err = BL_ERR;
    NVM_Operation_t op = NVM_NONE_OP;
    if (!writer.flags.start &&
        (err = Table_GetPartition(PARTITION_NEXT, &writer.node)) ==
        BL_OK && (err = NVM_GetSize(writer.node, &writer.size)) == BL_OK)
    {
        NVM_GetOperation(writer.node, &op);
        err = op == NVM_NONE_OP ? BL_OK : BL_ERR;
        if (err == BL_OK)
        {
            writer.flags.start = FLAG_SET;
            writer.flags.erase = FLAG_SET;
            err = BL_EINPROGRESS;
        }
    }
    if (writer.flags.erase)
    {
        if ((err = NVM_Erase(writer.node, writer.size)) == BL_OK)
        {
            NVM_OperationFinish(writer.node);
            writer.flags.erase = FLAG_CLEAR;
        }
    }
    else if (writer.flags.start)
    {
        err = BL_EALREADY;
    }
    return err;
}

BL_Err_t Writer_WriteData(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = !writer.flags.start ? BL_ERR : BL_EINVAL;
    if (data && length && err == BL_EINVAL)
    {
        err = NVM_Write(writer.node, data, length);
    }
    return err;
}

BL_Err_t Writer_Finish(void)
{
    BL_Err_t err = BL_EACCES;
    if (writer.flags.start)
    {
        NVM_OperationFinish(writer.node);
        writer = (struct writer_s) {0};
        err = BL_OK;
    }
    return err;
}

void Writer_States(BL_Err_t **err, BL_UINT8_T *count)
{
    if (err && count)
    {
        *err = (BL_Err_t *) acceptable;
        *count = ARRAY_SIZE(acceptable);
    }
}
