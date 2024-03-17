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
#include "table.h"
#include "nvm.h"
#include "helper.h"

static struct writer_s
{
    BL_UINT32_T size;
    NVM_Node_t node;
    struct flags_s
    {
        BL_UINT8_T start : 1;
        BL_UINT8_T read : 1;
        BL_UINT8_T write : 1;
        BL_UINT8_T : 5;
    } flags;
} writer = {0};

BL_Err_t Writer_Start(void)
{
    BL_Err_t err = BL_ERR;
    if ((err = Table_GetPartition(PARTITION_CURRENT, &writer.node)) == BL_OK &&
            (err = NVM_GetSize(writer.node, &writer.size)) == BL_OK)
    {
        writer.flags.start = FLAG_SET;
    }
    return err;
}

BL_Err_t Writer_Erase(void)
{
    BL_Err_t err = BL_ERR;
    if (writer.flags.start &&
        (err = NVM_Erase(writer.node, writer.size)) == BL_OK)
    {
        NVM_OperationFinish(writer.node);
    }
    return err;
}

BL_Err_t Writer_WriteData(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = writer.flags.read ? BL_ERR : BL_EINVAL;
    if (data && length && writer.flags.start && err == BL_EINVAL)
    {
        writer.flags.write = FLAG_SET;
        err = NVM_Write(writer.node, data, length);
    }
    return err;
}

BL_Err_t Writer_ReadData(BL_UINT8_T *data, BL_UINT32_T *length)
{
    BL_Err_t err = writer.flags.write ? BL_ERR : BL_EINVAL;
    if (data && length && writer.flags.start && err == BL_EINVAL)
    {
        writer.flags.read = FLAG_SET;
        err = NVM_Read(writer.node, data, length);
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
