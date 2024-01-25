/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup GD25Q80C
 * @{
 */

/**
 * @file        GD25Q80C.h
 *
 * @brief       Driver for GD25Q80C IC
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#ifndef __GD25Q80C_H
#define __GD25Q80C_H

#include "platform.h"

#define GD25Q80C_PAGE_SIZE (256U)
#define GD25Q80C_SECTOR_SIZE (4 * 1024U)
#define GD25Q80C_BLOCK_SIZE_32K (32 * 1024U)
#define GD25Q80C_BLOCK_SIZE_64K (64 * 1024U)
#define GD25Q80C_CHIP_SIZE (GD25Q80C_BLOCK_SIZE_64K * 16U)

#define GD25Q80C_CMD_SIZE (1U)
#define GD25Q80C_ADDRESS_SIZE (3U)
#define GD25Q80C_DUMMY_SIZE (1U)

#define GD25Q80C_CMD_PROG (0x2U)
#define GD25Q80C_CMD_READ (0x3U)
#define GD25Q80C_CMD_READ_STATUS (0x5U)
#define GD25Q80C_CMD_ERASE_SECTOR (0x20U)
#define GD25Q80C_CMD_ERASE_CHIP (0x60)
#define GD25Q80C_CMD_WEL (0x6U)
#define GD25Q80C_CMD_ENABLE_RESET (0x66U)
#define GD25Q80C_CMD_RESET (0x99U)
#define GD25Q80C_DEEP_POWER_DOWN (0xB9)
#define GD25Q80C_RELEASE_DEEP_POWER_DOWN (0xAB)


#define GD25Q80C_CMD_PROG_LENGTH (4U)
#define GD25Q80C_CMD_READ_LENGTH (4U)
#define GD25Q80C_CMD_ERASE_SECTOR_LENGTH (4U)
#define GD25Q80C_CMD_ERASE_CHIP_LENGTH (1U)
#define GD25Q80C_CMD_STATUS_LENGTH (1U)
#define GD25Q80C_CMD_WEL_LENGTH (1U)
#define GD25Q80C_CMD_ENABLE_RESET_LENGTH (1U)
#define GD25Q80C_CMD_RESET_LENGTH (1U)
#define GD25Q80C_DEEP_POWER_DOWN_LENGTH (1U)
#define GD25Q80C_RELEASE_DEEP_POWER_DOWN_LENGTH (1U)

typedef enum
{
    GD25Q80C_OK = 0x0,
    GD25Q80C_ERROR = 0x01,
    GD25Q80C_INVALID_PARAM = 0x02,
    GD25Q80C_TIMEOUT = 0x04,
    GD25Q80C_EXCEED_BOUNDRY = 0x08,
    GD25Q80C_TRANSFER_ONGOING = 0x10,
} GD25Q80C_Status_t;

typedef struct
{
    uint8_t *cmd;
    uint8_t cSize;
    uint8_t *tx;
    uint8_t *rx;
    uint32_t dSize;
} GD25Q80C_Data_t;

typedef GD25Q80C_Status_t (*GD25Q80C_SPI_Transfer_t) (GD25Q80C_Data_t data);
typedef void (*GD25Q80C_DelayUs_t) (uint32_t us);
typedef void (*GD25Q80C_Atomic_Enter_t) (void);
typedef void (*GD25Q80C_Atomic_Exit_t) (void);

typedef struct
{
    GD25Q80C_SPI_Transfer_t transfer;
    GD25Q80C_DelayUs_t delay;
    GD25Q80C_Atomic_Enter_t enter;
    GD25Q80C_Atomic_Exit_t exit;
} GD25Q80C_Config_t;

typedef struct
{
    GD25Q80C_Config_t cfg;
    uint8_t wait : 1U;
} GD25Q80C_Inst_t;

/**
 * @brief   Initialize a GD25Q80C instance
 *
 * @details Creates an instance for the GD25Q80C in accordance to the config
 *          passed in.
 * 
 * @note    This must be called before to any other API in this module.
 *
 * @param inst Pointer to the instance that will represent the chip driver
 * @param cfg Configuration structure
 * 
 * @return GD25Q80C_OK, on success
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 */
GD25Q80C_Status_t GD25Q80C_Init(GD25Q80C_Inst_t *inst, GD25Q80C_Config_t cfg);

/**
 * @brief   Write to a specific address
 *
 * @details Writes data of size in bytes to a specific address.
 * 
 * @note    The address plus the size written must be erased before
 *          programming the chip.
 * 
 * @note    The program operation will not extend past a 256 byte boundry,
 *          ensure that the data passed in will not exceed this boundry.
 *
 * @param inst Pointer to the instance that will represent the chip driver
 * @param address Address to program to, must be below GD25Q80C_CHIP_SIZE
 * @param data Pointer to the data to be programmed
 * @param size Size of the data to be programmed
 * 
 * @return GD25Q80C_OK, on success
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 * @return GD25Q80C_TIMEOUT, there is a timeout while waiting on an operation
 * @return GD25Q80C_EXCEED_BOUNDRY, the data exceeds the byte boundry
 */
GD25Q80C_Status_t GD25Q80C_Program(GD25Q80C_Inst_t *inst,
                                     uint32_t address,
                                     uint8_t *data,
                                     uint16_t size);

/**
 * @brief   Read from a specific address
 *
 * @details Reads data of size in bytes from a specific address.
 * 
 * @param inst Pointer to the instance that will represent the chip driver
 * @param address Address to read from, must be below GD25Q80C_CHIP_SIZE
 * @param data Pointer for the data to be read
 * @param size Size of the data to be read
 * 
 * @return GD25Q80C_OK, on success
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 * @return GD25Q80C_TIMEOUT, there is a timeout while waiting on an operation
 * @return GD25Q80C_EXCEED_BOUNDRY, the data exceeds the byte boundry
 */
GD25Q80C_Status_t GD25Q80C_Read(GD25Q80C_Inst_t *inst,
                                    uint32_t address,
                                    uint8_t *data,
                                    uint16_t size);

/**
 * @brief   Erase a sector
 *
 * @details Erases a sector specified by GD25Q80C_SECTOR_SIZE
 * 
 * @note    Sectors are all GD25Q80C_SECTOR_SIZE
 * 
 * @param inst Pointer to the instance that will represent the chip driver
 * @param sector Sector number to be erased
 * 
 * @return GD25Q80C_OK, on success
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 * @return GD25Q80C_TIMEOUT, there is a timeout while waiting on an operation
 * @return GD25Q80C_EXCEED_BOUNDRY, the data exceeds the byte boundry
 */
GD25Q80C_Status_t GD25Q80C_EraseSector(GD25Q80C_Inst_t *inst, uint16_t sector);

/**
 * @brief   Erase the chip
 *
 * @details Erases the chip and all of its data
 * 
 * @note    This operation takes up to 3 seconds
 * 
 * @param inst Pointer to the instance that will represent the chip driver
 * 
 * @return GD25Q80C_OK, on success
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 * @return GD25Q80C_TIMEOUT, there is a timeout while waiting on an operation
 */
GD25Q80C_Status_t GD25Q80C_EraseChip(GD25Q80C_Inst_t *inst);

/**
 * @brief   Reset the chip
 *
 * @details Resets the chip
 * 
 * @param inst Pointer to the instance that will represent the chip driver
 * 
 * @return GD25Q80C_OK, on success
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 * @return GD25Q80C_TIMEOUT, there is a timeout while waiting on an operation
 */
GD25Q80C_Status_t GD25Q80C_Reset(GD25Q80C_Inst_t *inst);

/**
 * @brief   Place the chip in sleep mode
 *
 * @details Places the chip in deep-sleep mode
 * 
 * @param inst Pointer to the instance that will represent the chip driver
 * 
 * @return GD25Q80C_OK, on success
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 * @return GD25Q80C_TIMEOUT, there is a timeout while waiting on an operation
 */
GD25Q80C_Status_t GD25Q80C_Sleep(GD25Q80C_Inst_t *inst);

/**
 * @brief   Wake up the chip
 *
 * @details Wakes up the chip from deep-sleep mode
 * 
 * @param inst Pointer to the instance that will represent the chip driver
 * 
 * @return GD25Q80C_OK, on success
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 * @return GD25Q80C_TIMEOUT, there is a timeout while waiting on an operation
 */
GD25Q80C_Status_t GD25Q80C_WakeUp(GD25Q80C_Inst_t *inst);

/**
 * @brief   Call when a transfer has been completed
 *
 * @details API used to determine when a transfer has completed. This is useful
 *          in systems that may use DMA to transfer data to this chipset. This
 *          ensures that new data will not be sent when there is an ongoing
 *          transaction
 * 
 * @note    This must be called when a transaction (rx/tx) is completed in
 *          order to ensure that operations will work as intended.
 * 
 * @param inst Pointer to the instance that will represent the chip driver
 */
void GD25Q80C_TransferComplete(GD25Q80C_Inst_t *inst);

/**
 * @brief   Returns the transfer status of the chip
 *
 * @details API used to getr when a transfer has completed. This is useful
 *          in systems that may use DMA to transfer data to this chipset.
 * 
 * @note    This must be called before a transaction (rx/tx) is started in
 *          order to ensure that operations will work as intended.
 * 
 * @param inst Pointer to the instance that will represent the chip driver
 * 
 * @return GD25Q80C_OK, on success/transfer is complete
 * @return GD25Q80C_INVALID_PARAM, on invalid passed parameters
 * @return GD25Q80C_TRANSFER_ONGOING, transfer is still in progress
 */
GD25Q80C_Status_t GD25Q80C_GetTransferStatus(GD25Q80C_Inst_t *inst);

#endif // __GD25Q80C_H

/**@} GD25Q80C */
