/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup flash
 * @{
 */

/**
 * @file        flash.h
 *
 * @brief       Provides API for Internal Flash Operations
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-14
 */

/*****************************************************************************/
#ifndef __FLASH_H
#define __FLASH_H

#include "platform.h"

void Flash_Init(void);
void Flash_Deinit(void);
void Flash_Write(uint32_t address, uint8_t *data, uint32_t size);
void Flash_Erase(uint32_t address, uint32_t size);
void Flash_Read(uint32_t address, uint8_t *data, uint32_t size);

#endif // __FLASH_H

/**@} flash */