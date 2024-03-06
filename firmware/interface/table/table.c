/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**************************************************************************//**
 * @file        table.c
 *
 * @brief       Provides an interface to write/read data from the partition
 *              table. The partition table is responsible for providing
 *              information about the partitions on the device. These would
 *              include which partition will be loaded into the application
 *              space, the checksum of that partition as well as the signature
 *              to verify against when attempting to load the application into
 *              the application space.
 *
 * @author      Matthew Krause
 *
 * @date        2024-03-03
 *****************************************************************************/
#include "table.h"
#include "helper.h"
#include "nvm.h"

#define NUM_DEFAULT_NODES (2)
#define NUM_LOADABLE_PARTITIONS (NUM_NVM_NODES - NUM_DEFAULT_NODES)
#define LOADABLE_PARTITION_OFFSET(x) (x + NUM_DEFAULT_NODES)
#define PARTITION_TABLE_MAX_INC_CHECK(x, y) \
    x = y + 1 < NUM_LOADABLE_PARTITIONS ? y + 1 : y
#define UPDATE_LUT() \
    helper.lut[PARTITION_CURRENT] = info.table.partition.current; \
    helper.lut[PARTITION_PREVIOUS] = info.table.partition.previous; \
    PARTITION_TABLE_MAX_INC_CHECK(helper.lut[PARTITION_NEXT], \
                                  info.table.partition.current);

#define POLL_NVM_OPERATION(err, op) \
    POLL_DMA_FUNCTION(err, op); \
    NVM_OperationFinish(PARTITION_NODE);

BL_STATIC struct __attribute__((__packed__))
{
    Table_t table;
    Table_Partition_t partitions[NUM_LOADABLE_PARTITIONS];
} info = {0};

BL_STATIC struct
{
    BL_UINT32_T lut[PARTITION_COUNT];
    BL_UINT32_T size;
} helper = {0};

BL_STATIC BL_Err_t table_EraseAndWrite(void);

BL_Err_t Table_Init(void)
{
    BL_Err_t err = BL_ERR;
    BL_UINT32_T size = sizeof(info);
    err = NVM_GetSize(PARTITION_NODE, &helper.size);

    if (err == BL_OK)
    {
        POLL_NVM_OPERATION(err, NVM_Read(PARTITION_NODE,
                                         (BL_UINT8_T *) &info,
                                         &size));
        if (err == BL_OK && info.table.magic != TABLE_MAGIC)
        {
            MEMSET(&info, 0, sizeof(info));
            info.table.magic = TABLE_MAGIC;
            info.table.partition.current = 0;
            info.table.partition.previous = 0;
            for (BL_UINT32_T i = 0; i < NUM_LOADABLE_PARTITIONS; i++)
            {
                info.partitions[i].magic = PARTITION_MAGIC;
            }
            err = table_EraseAndWrite();
        }
        UPDATE_LUT();
    }
    return err;
}

void Table_Deinit(void)
{
    MEMSET(&info, 0, BL_SIZEOF(info));
    MEMSET(&helper, 0, BL_SIZEOF(helper));
}

BL_Err_t Table_WritePartiton(Table_Types_e table,
                             Table_Partition_t *partition)
{
    BL_Err_t err = BL_EIO;
    if (table < PARTITION_COUNT &&
        partition &&
        helper.lut[table] < NUM_LOADABLE_PARTITIONS)
    {
        MEMCPY(&info.partitions[helper.lut[table]],
               partition,
               BL_SIZEOF(Table_Partition_t));
        err = table_EraseAndWrite();
    }
    return err;
}

BL_Err_t Table_ReadPartiton(Table_Types_e table,
                            Table_Partition_t *partition)
{
    BL_Err_t err = BL_EIO;
    BL_UINT32_T size = BL_SIZEOF(info);
    if (table < PARTITION_COUNT &&
        partition &&
        helper.lut[table] < NUM_LOADABLE_PARTITIONS)
    {
        POLL_NVM_OPERATION(err, NVM_Read(PARTITION_NODE,
                                         (BL_UINT8_T *) &info,
                                         &size));

        if (err == BL_OK)
        {
            MEMCPY(partition,
                   &info.partitions[helper.lut[table]],
                   BL_SIZEOF(Table_Partition_t));
        }
    }
    return err;
}

BL_Err_t Table_RevertPartitions(void)
{
    BL_Err_t err = BL_ERR;
    if (info.table.partition.current != info.table.partition.previous)
    {
        info.table.partition.current = info.table.partition.previous;
        UPDATE_LUT();
        err = table_EraseAndWrite();
    }

    return err;
}
BL_Err_t Table_UpdatePartitions(void)
{
    BL_Err_t err = BL_ERR;
    info.table.partition.previous = info.table.partition.current;
    PARTITION_TABLE_MAX_INC_CHECK(info.table.partition.current,
                                  info.table.partition.current);
    UPDATE_LUT();
    err = table_EraseAndWrite();

    return err;
}

BL_Err_t Table_CheckValid(Table_Types_e table)
{
    BL_Err_t err = table < PARTITION_COUNT ? BL_ERR : BL_EIO;
    if (err == BL_ERR &&
        info.partitions[helper.lut[table]].magic == PARTITION_MAGIC)
    {
        err = BL_OK;
    }
    return err;
}

BL_STATIC BL_Err_t table_EraseAndWrite(void)
{
    BL_Err_t err = BL_ERR;
    POLL_NVM_OPERATION(err, NVM_Erase(PARTITION_NODE, helper.size));
    POLL_NVM_OPERATION(err, NVM_Write(PARTITION_NODE,
                                      (BL_UINT8_T *) &info,
                                      BL_SIZEOF(info)));
    return err;
}
