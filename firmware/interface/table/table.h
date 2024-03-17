/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_TABLE_H
#define __BL_TABLE_H

/**
 * @addtogroup table
 * @{
 */

/**************************************************************************//**
 * @file        table.h
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
#include "config.h"
#include "verify.h"
#include "nvm.h"

#define PARTITION_MAGIC 0xCAFE
#define TABLE_MAGIC 0xBABE

/* Data must be 16 byte aligned in the following structs for AES operations */
typedef struct __attribute__((__packed__))
{
    BL_UINT16_T magic;
    BL_UINT32_T length;
    BL_UINT32_T reserved1;
    BL_UINT32_T crc;
    BL_UINT32_T reserved2;
    BL_UINT8_T signature[VERIFY_SIGNATURE_LENGTH];
    BL_UINT8_T reserved3[14];
} Table_Partition_t;

typedef struct __attribute__((__packed__))
{
    BL_UINT8_T reserved[6];
    BL_UINT16_T magic;
    struct __attribute__((__packed__))
    {
        BL_UINT32_T current;
        BL_UINT32_T previous;
    } partition;
} Table_t;

typedef enum
{
    PARTITION_CURRENT,
    PARTITION_PREVIOUS,
    PARTITION_NEXT,
    PARTITION_COUNT,
} Table_Types_e;


/******************************************************************************
 * @brief Initialize Partition Table
 *
 * @details If partition table exists on machine, this will update metadata
 *          used for loading, flashing and reading information about specific
 *          partitions. If the partition table does not exist on the machine
 *          this will create a default partition table and load it into the
 *          PARTITION_NODE found in the NVM configuration.
 *
 * @return BL_OK on success
 * @return BL_X on failure
 *****************************************************************************/
BL_Err_t Table_Init(void);

/******************************************************************************
 * @brief Deintialize Partition Table
 *****************************************************************************/
void Table_Deinit(void);

/******************************************************************************
 * @brief Write Partition Information
 *
 * @details Writes partition information for one of Table_Types_e. The table
 *          interface must be initialized before calling this API. This is
 *          intended to be used to write new firmware information about a
 *          partition.
 *
 * @param table table type to write to
 * @param partition partition information
 *
 * @return BL_OK on success
 * @return BL_EIO on invalid parameters
 * @return BL_X on failure
 *****************************************************************************/
BL_Err_t Table_WritePartiton(Table_Types_e table,
                             Table_Partition_t *partition);

/******************************************************************************
 * @brief Reads Partition Information
 *
 * @details Reads partition information from one of Table_Types_e. The table
 *          interface must be initialized before calling this API. This is
 *          intended to be used when loading firmware.
 *
 * @param table table type to read from
 * @param partition partition information
 *
 * @return BL_OK on success
 * @return BL_EIO on invalid parameters
 * @return BL_X on failure
 *****************************************************************************/
BL_Err_t Table_ReadPartiton(Table_Types_e table,
                            Table_Partition_t *partition);

/******************************************************************************
 * @brief Get The Requested Node For The Partition
 *
 * @details Obtains the current node for the partition requested.
 *          Will fail if the module has not been initialized.
 *
 * @param table table type to obtain
 * @param node node of requested type
 *
 * @return BL_OK on success
 * @return BL_ERR if module not initialized
 *****************************************************************************/
BL_Err_t Table_GetPartition(Table_Types_e table, NVM_Node_t *node);

/******************************************************************************
 * @brief Revert Current Partition
 *
 * Used to revert the current partition if the loaded partition is invalid.
 * If the current partition is the same as the previous partition this API will
 * fail.
 *
 * @return BL_OK on success
 * @return BL_ERR when partition cannot be reverted
 * @return BL_X on failure to write partition table
 *****************************************************************************/
BL_Err_t Table_RevertPartitions(void);

/******************************************************************************
 * @brief Update Current Partition
 *
 * Used to update the current partition if new firmware is to be loaded onto
 * the device.
 *
 * @return BL_OK on success
 * @return BL_X on failure to write partition table
 *****************************************************************************/
BL_Err_t Table_UpdatePartitions(void);

/******************************************************************************
 * @brief Check If Table Is Valid
 *
 * @param table table type
 *
 * @return BL_OK on success
 * @return BL_EIO on invalid parameters
 * @return BL_X on failure
 *****************************************************************************/
BL_Err_t Table_CheckValid(Table_Types_e table);

/**@} table */

#endif //__BL_TABLE_H
