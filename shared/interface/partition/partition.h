/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __PARTITION_H
#define __PARTITION_H

/**
 * @addtogroup partition
 * @{
 */

/**************************************************************************//**
 * @file        partition.h
 *
 * @brief       Defines the partitions necessary for the application and
 *              bootloader on the external flash chip
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-09
 *****************************************************************************/
#include "platform.h"
#include "GD25Q80C.h"

#ifdef __GD25Q80C_H
#define PARTITION_WRITE_SIZE GD25Q80C_PAGE_SIZE
#define PARTITION_SECTOR_SIZE GD25Q80C_SECTOR_SIZE
#endif // __GD25Q80C_H
#define PARTITION_UPDATE_SIZE (0x20000U)

typedef enum
{
    PARTITION_NONE = -1,
    PARTITION_TABLE = 0U,
    PARTITION_UPDATE_1,
    PARTITION_UPDATE_2,
    PARTITION_NUM,
} Partitions_t;

typedef enum
{
    PARTITION_TABLE_LOCATION = 0x00000000,
    PARTITION_UPDATE_1_LOCATION = PARTITION_SECTOR_SIZE,
    PARTITION_UPDATE_2_LOCATION = PARTITION_SECTOR_SIZE + PARTITION_UPDATE_SIZE,
} Partition_Locations_t;

typedef enum
{
    PARTITION_TABLE_SIZE = PARTITION_SECTOR_SIZE,
    PARTITION_UPDATE_1_SIZE = PARTITION_UPDATE_SIZE,
    PARTITION_UPDATE_2_SIZE = PARTITION_UPDATE_SIZE,
} Partition_Size_t;

void Partition_Init(void);
bool Partition_Write(uint32_t address, uint8_t *data, uint32_t length);
bool Partition_Read(uint32_t address, uint8_t *data, uint32_t length);
bool Partition_Erase(uint32_t address, uint32_t length);

/**@} partition */

#endif //__PARTITION_H