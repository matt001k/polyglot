/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup flash
 * @{
 */

/**
 * @file        ext_flash.c
 *
 * @brief       Provides the API for Flash Storage
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#include "ext_flash.h"
#include "GD25Q80C.h"
#include "delay.h"
#include "spi.h"
#include "atomic.h"
#include "gpio.h"

#define FLASH_BUF_SIZE ((GD25Q80C_PAGE_SIZE * 4U) + GD25Q80C_CMD_SIZE + \
                          GD25Q80C_ADDRESS_SIZE)
#define CONVERT_ADDRESS_TO_SECTOR_OFFSET(address, sector, offset)   \
do                                                                  \
{                                                                   \
    sector = address / GD25Q80C_SECTOR_SIZE;                        \
    offset = address % GD25Q80C_SECTOR_SIZE;                        \
} while (0U)


static void spi_TxDone(void);
static void spi_RxDone(void);
static GD25Q80C_Status_t spi_Transfer(GD25Q80C_Data_t data);

static uint8_t rxBuf[FLASH_BUF_SIZE] = {0U};
static uint8_t txBuf[FLASH_BUF_SIZE] = {0U};
static GD25Q80C_Inst_t gInst = {0U};

void Ext_FlashInit(void)
{
    GD25Q80C_Config_t cfg =
    {
        spi_Transfer,
        Delay_Us,
        (GD25Q80C_Atomic_Enter_t) Atomic_EnterCritical,
        (GD25Q80C_Atomic_Exit_t) Atomic_ExitCritical,
    };

    GPIO_Set(FLASH_WP);
    GD25Q80C_Init(&gInst, cfg);

    SPI_RegisterTxCallback(SPI_FLASH, spi_TxDone);
    SPI_RegisterRxCallback(SPI_FLASH, spi_RxDone);
}

void Ext_FlashWrite(uint32_t address, uint8_t *data, uint16_t size)
{
    uint32_t length = 0U;
    uint16_t sector = 0U;
    uint16_t offset = 0U;
    CONVERT_ADDRESS_TO_SECTOR_OFFSET(address, sector, offset);

    while (size)
    {
        if (Ext_FlashGetState() != FLASH_TRANSFER_STATE_IN_PROGRESS)
        {
            length = GD25Q80C_PAGE_SIZE - (offset % GD25Q80C_PAGE_SIZE);
            if (length > size)
            {
                length = size;
            }
            GD25Q80C_Program(&gInst,
                             sector * GD25Q80C_SECTOR_SIZE + offset,
                             data,
                             length);
            offset += length;
            if (offset >= GD25Q80C_SECTOR_SIZE)
            {
                offset = 0U;
                sector++;
            }
            size -= length;
            data += length;
        }
    }
}

void Ext_FlashStartRead(uint32_t address, uint16_t size)
{
    while (Ext_FlashGetState() == FLASH_TRANSFER_STATE_IN_PROGRESS) {};
    GD25Q80C_Read(&gInst, address, rxBuf, size);
}

void Ext_FlashGetRead(uint8_t *data, uint16_t size)
{
    memcpy(data, rxBuf + GD25Q80C_CMD_READ_LENGTH, size);
}

Flash_State_t Ext_FlashGetState(void)
{
    Flash_State_t state = FLASH_TRANSFER_STATE_IN_PROGRESS;

    ENTER_CRITICAL();
    state = GD25Q80C_GetTransferStatus(&gInst) == GD25Q80C_OK ?
            FLASH_TRANSFER_STATE_DONE : state;
    EXIT_CRITICAL();

    return state;
}

void Ext_FlashEraseSector(uint16_t sector)
{
    GD25Q80C_EraseSector(&gInst, sector);
}

static void spi_TxDone(void)
{
}

static void spi_RxDone(void)
{
    SPI_DeassertCs(FLASH_CS);
    GD25Q80C_TransferComplete(&gInst);
}

static GD25Q80C_Status_t spi_Transfer(GD25Q80C_Data_t data)
{
    GD25Q80C_Status_t status = GD25Q80C_OK;
    uint8_t *rxPtr = rxBuf;

    memcpy(txBuf, data.cmd, data.cSize);
    if (data.tx)
    {
        memcpy(txBuf + data.cSize, data.tx, data.dSize);
    }
    else if (data.dSize)
    {
        memset(txBuf + data.cSize, 0xFF, data.dSize);
    }

    if (data.cmd[0U] == GD25Q80C_CMD_READ_STATUS)
    {
        rxPtr = data.rx;
    }

    SPI_AssertCs(FLASH_CS);
    SPI_TransmitReceive(SPI_FLASH, txBuf, rxPtr, data.cSize + data.dSize);
    
    return status;
}

/**@} flash */