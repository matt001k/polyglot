/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup bootloader
 * @{
 */

/**
 * @file        bootloader.c
 *
 * @brief       Provides the API for Bootloader Configuration
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-02S
 */

/*****************************************************************************/
#include "bootloader.h"

#define BOOTLOADER_KEY_WORD (0xBADBABE9)

RAM_SHARED volatile uint32_t bootloader_key;
extern uint32_t __shared_start__;
extern uint32_t __shared_end__;

bool Bootloader_Get(void)
{
    bool ret = false;

    if (bootloader_key == BOOTLOADER_KEY_WORD)
    {
        ret = true;
    }

    return ret;
}

void Bootloader_Set(void)
{
    bootloader_key = BOOTLOADER_KEY_WORD;

    //JUMP_TO_BOOT(BOOT_LOCATION);
}

/**@} bootloader */
