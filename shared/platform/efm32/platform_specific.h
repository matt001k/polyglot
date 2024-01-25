/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup platform
 * @{
 */

/**
 * @file        platform_specific.h
 *
 * @brief       Provides the platform dependent configuration includes, defines
 *              etc.
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#ifndef __PLATFORM_SPECIFIC_H
#define __PLATFORM_SPECIFIC_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "efm32tg11b520f128gm64.h"
#include "cmsis_gcc.h"

#define APP_LOCATION_SPECIFIC (uint32_t)& __approm_start__
#define APP_SIZE_SPECIFIC (uint32_t)& __approm_size__
#define BOOT_LOCATION_SPECIFIC (uint32_t)&__bootrom_start__
#define BOOT_SIZE_SPECIFIC (uint32_t)& __bootrom_size__
#define MCU_SECTOR_SIZE_SPECIFIC 0x800U
#define RAM_FUNC_SPECIFIC __attribute__ ((section(".ram")))
#define RAM_SHARED_SPECIFIC __attribute__((section(".shared")))
#define JUMP_TO_APP_SPECIFIC Jump_ToLocation
#define JUMP_TO_BOOT_SPECIFIC Jump_ToLocation

static inline void Jump_ToLocation(uint32_t address);

extern int __approm_start__;
extern int __approm_size__;
extern int __bootrom_start__;
extern int __bootrom_size__;

static inline void Jump_ToLocation(uint32_t address)
{
    void (*jump)(void);
    uint32_t *p = (uint32_t *) address;
    jump = (void (*)(void)) (p[1]);
    //__disable_irq();
    SCB->VTOR = address;
    __set_MSP(p[0]);
    jump();
}

#endif //__PLATFORM_SPECIFIC_H