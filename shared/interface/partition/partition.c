/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup partition
 * @{
 */

/**************************************************************************//**
 * @file        partition.h
 *
 * @brief       Defines the partitions necessary for the application and
 *              bootloader
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-09
 *****************************************************************************/
#include "partition.h"
#include "ext_flash.h"

#define PARTITION_SET (1U)
#define PARTITION_CLEAR (0U)

#define CLEAR_TRANSACTION_ONGOING(p) (partitions[p].ongoing = PARTITION_CLEAR)
#define SET_TRANSACTION_ONGOING(p) (partitions[p].ongoing = PARTITION_SET)
#define GET_TRANSACTION_ONGOING(p) (partitions[p].ongoing == PARTITION_SET)
#define PARTITION_VALIDATE(p) (p != PARTITION_NONE)
#define GET_PARTITION_LENGTH(length, p)                             \
do                                                                  \
{                                                                   \
    length = ((address + length) < (partitions[p].size +            \
             partitions[p].location)) ? length :                    \
             partitions[p].size + partitions[p].location - address; \
} while (0U)
#define CONVERT_ADDRESS_TO_SECTOR(address, sector)   \
do                                                   \
{                                                    \
    sector = address / PARTITION_SECTOR_SIZE;        \
} while (0U)
#define ASSIGN_ERASE_LENGTH(length, p)          \
do                                              \
{                                               \
    if (!partitions[p].erase_length)            \
    {                                           \
        partitions[p].erase_length = length;    \
    }                                           \
} while (0U)
#define ASSIGN_WRITE_LENGTH(length, p)          \
do                                              \
{                                               \
    if (!partitions[p].write_length)            \
    {                                           \
        partitions[p].write_length = length;    \
    }                                           \
} while (0U)



typedef struct
{
    Partition_Locations_t location;
    Partition_Size_t size;
    uint32_t erase_length;
    uint32_t write_length;
    uint8_t ongoing : 1U;
} partition_t;

static partition_t partitions[] =
{
    [PARTITION_TABLE] =
    {
        PARTITION_TABLE_LOCATION,
        PARTITION_TABLE_SIZE,
        0U,
        PARTITION_CLEAR,
    },
    [PARTITION_UPDATE_1] =
    {
        PARTITION_UPDATE_1_LOCATION,
        PARTITION_UPDATE_1_SIZE,
        0U,
        PARTITION_CLEAR,
    },
    [PARTITION_UPDATE_2] =
    {
        PARTITION_UPDATE_2_LOCATION,
        PARTITION_UPDATE_2_SIZE,
        0U,
        PARTITION_CLEAR,
    },
};

static inline bool partition_Lookup(uint32_t address, Partitions_t *p);

void Partition_Init(void)
{
    static uint8_t init = PARTITION_CLEAR;

    if (!init)
    {
        Ext_FlashInit();
        init = PARTITION_SET;
    }
}

bool Partition_Write(uint32_t address, uint8_t *data, uint32_t length)
{
    bool ret = false;
    Partitions_t p = (Partitions_t) 0U;

    if (Ext_FlashGetState() == FLASH_TRANSFER_STATE_DONE)
    {
        if (partition_Lookup(address, &p) && PARTITION_VALIDATE(p))
        {
            if (GET_TRANSACTION_ONGOING(p))
            {
                CLEAR_TRANSACTION_ONGOING(p);
                ret = true;
            }
            else
            {
                GET_PARTITION_LENGTH(length, p);
                Ext_FlashWrite(address, data, length);
                SET_TRANSACTION_ONGOING(p);
            }
        }
    }

    return ret;
}

bool Partition_Read(uint32_t address, uint8_t *data, uint32_t length)
{
    bool ret = false;
    Partitions_t p = (Partitions_t) 0U;

    if (Ext_FlashGetState() == FLASH_TRANSFER_STATE_DONE)
    {
        if (partition_Lookup(address, &p) && PARTITION_VALIDATE(p))
        {
            if (GET_TRANSACTION_ONGOING(p))
            {
                GET_PARTITION_LENGTH(length, p);
                Ext_FlashGetRead(data, length);
                CLEAR_TRANSACTION_ONGOING(p);
                ret = true;
            }
            else
            {
                Ext_FlashStartRead(address, length);
                SET_TRANSACTION_ONGOING(p);
            }
        }
    }

    return ret;
}

bool Partition_Erase(uint32_t address, uint32_t length)
{
    bool ret = false;
    Partitions_t p = (Partitions_t) 0U;
    uint16_t sector = 0U;

    if (Ext_FlashGetState() == FLASH_TRANSFER_STATE_DONE)
    {
        if (partition_Lookup(address, &p) && PARTITION_VALIDATE(p))
        {
            if (GET_TRANSACTION_ONGOING(p) && !partitions[p].erase_length)
            {
                CLEAR_TRANSACTION_ONGOING(p);
                ret = true;
            }
            else
            {
                ASSIGN_ERASE_LENGTH(length, p);
                sector = (address + partitions[p].erase_length) / PARTITION_SECTOR_SIZE;
                Ext_FlashEraseSector(sector - 1U);
                SET_TRANSACTION_ONGOING(p);
                partitions[p].erase_length = partitions[p].erase_length >=
                                             PARTITION_SECTOR_SIZE ?
                                             partitions[p].erase_length -
                                             PARTITION_SECTOR_SIZE :
                                             0U;
            }
        }
    }

    return ret;
}

static inline bool partition_Lookup(uint32_t address, Partitions_t *p)
{
    bool ret = false;

    for (Partitions_t pIdx = 0U; pIdx < PARTITION_NUM; pIdx++)
    {
        if (address < (partitions[pIdx].location + partitions[pIdx].size))
        {
            *p = pIdx;
            ret = true;
            break;
        }
    }

    if (ret == false)
    {
        *p = PARTITION_NONE;
    }

    return ret;
}

/**@} partition */
