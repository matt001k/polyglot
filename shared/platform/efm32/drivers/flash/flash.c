/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup flash
 * @{
 */

/**
 * @file        flash.c
 *
 * @brief       Provides API for Internal Flash Operations
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-14
 */

/*****************************************************************************/
#include "flash.h"
#include "atomic.h"

/* Flash sector size */
#define FLASH_SECTOR_SIZE (0x800U)

/* Macro to unlock the MSC */
#define UNLOCK_MSC() (MSC->LOCK = 0x1B71U)
/* Macro to lock the MSC */
#define LOCK_MSC() (MSC->LOCK = 0U)
/* Macro to wait while an erase or write operation is in progress */
#define MSC_WAIT()                      \
do                                      \
{                                       \
                                        \
}                                       \
while (MSC->STATUS & MSC_STATUS_BUSY)

/* Linker script variable to determine the end of flash */
extern int __flash_end__;

RAM_FUNC void f_RunCmd(uint32_t cmd);

void Flash_Init(void)
{
    ENTER_CRITICAL();
    /* Unlock the MSC and enable erases and writes */
    UNLOCK_MSC();
    MSC->WRITECTRL = MSC_WRITECTRL_WREN;
    LOCK_MSC();
    EXIT_CRITICAL();
}

void Flash_Deinit(void)
{
    ENTER_CRITICAL();
    /* Unlock the MSC and disables erases and writes */
    UNLOCK_MSC();
    MSC->WRITECTRL = 0U;
    LOCK_MSC();
    EXIT_CRITICAL();
}

void Flash_Write(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t buf = 0U;
    if (data && size && (address + size) <= (uint32_t)&__flash_end__)
    {
        /* Unlock the MSC and wait for any ongoing operations */
        UNLOCK_MSC();
        MSC_WAIT();

        /* Continuously write words to internal flash */
        while (size)
        {
            /* Convert uint8 to uint32 little-endian */
            buf = *((uint32_t *) data);
            if (size >= 4U)
            {
                data += 4U;
                size -= 4U;
            }
            else if (size == 3U)
            {
                buf &= ~0xFF000000;
                data += 3U;
                size -= 3U;
            }
            else if (size == 2U)
            {
                buf &= ~0xFFFF0000;
                data += 2U;
                size -= 2U;
            }
            else
            {
                buf &= ~0xFFFFFF00;
                data++;
                size--;
            }

            /* Load the address to be written to and the word */
            while (!(MSC->STATUS & MSC_STATUS_WDATAREADY)) {};
            MSC->ADDRB = address;
            MSC->WDATA = buf;
            MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

            /* Send command to write the internal memory address */
            f_RunCmd(MSC_WRITECMD_WRITEONCE);

            /* Increment the address to write to */
            address += 4U;
        }

        /* Lock the internal flash registers */
        LOCK_MSC();
    }
}

void Flash_Erase(uint32_t address, uint32_t size)
{
    /* Unlock the MSC and wait for any ongoing operations */
    UNLOCK_MSC();
    MSC_WAIT();

    if ((address + size) <= (uint32_t)&__flash_end__)
    {
        while (size)
        {
            /* Determine the size of the erase operation and decrement */
            if (size > FLASH_SECTOR_SIZE)
            {
                size -= FLASH_SECTOR_SIZE;
            }
            else
            {
                size -= size;
            }

            /* Load the address to perform the erase operation */
            MSC->ADDRB = address;
            MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

            /* Send command to perform the erase operation */
            ENTER_CRITICAL();
            f_RunCmd(MSC_WRITECMD_ERASEPAGE);
            EXIT_CRITICAL();

            /* Increment the address */
            address += FLASH_SECTOR_SIZE;
        }
    }

    /* Lock the internal flash registers */
    LOCK_MSC();
}

void Flash_Read(uint32_t address, uint8_t *data, uint32_t size)
{
    if (data && size && (address + size) <= (uint32_t)&__flash_end__)
    {
        /* Basically a memcpy of the required address to data */
        while (size--)
        {
            *data++ = *((uint8_t *) address);
            address++;
        }
    }
}

RAM_FUNC void f_RunCmd(uint32_t cmd)
{
    MSC->WRITECMD = cmd;
    MSC_WAIT();
}

/**@} flash */