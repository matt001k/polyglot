/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup ext_flash
 * @{
 */

/**
 * @file        ext_flash.h
 *
 * @brief       Provides the API for External Flash Storage
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#ifndef __EXT_FLASH_H
#define __EXT_FLASH_H

#include "platform.h"

typedef enum
{
    FLASH_TRANSFER_STATE_DONE = 0x0,
    FLASH_TRANSFER_STATE_IN_PROGRESS,
} Flash_State_t;

void Ext_FlashInit(void);
void Ext_FlashWrite(uint32_t address, uint8_t *data,uint16_t size);
void Ext_FlashStartRead(uint32_t address, uint16_t size);
void Ext_FlashGetRead(uint8_t *data, uint16_t size);
Flash_State_t Ext_FlashGetState(void);
void Ext_FlashEraseSector(uint16_t sector);

#endif // __EXT_FLASH_H

/**@} ext_flash */