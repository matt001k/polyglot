/**************************************************************************/ /**
                                                                              * (c) 2022 Ahriman
                                                                              * This code is licensed under MIT license (see LICENSE.txt for details)
                                                                              *****************************************************************************/

/**
 * @addtogroup platform
 * @{
 */

/**
 * @file        platform.h
 *
 * @brief       Provides the platform dependent configuration includes, defines
 *              etc.
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "platform_specific.h"

#define APP_LOCATION APP_LOCATION_SPECIFIC
#define APP_SIZE APP_SIZE_SPECIFIC
#define BOOT_LOCATION BOOT_LOCATION_SPECIFIC
#define BOOT_SIZE BOOT_SIZE_SPECIFIC
#define MCU_SECTOR_SIZE MCU_SECTOR_SIZE_SPECIFIC
#define RAM_FUNC RAM_FUNC_SPECIFIC
#define RAM_SHARED RAM_SHARED_SPECIFIC
#define JUMP_TO_APP JUMP_TO_APP_SPECIFIC
#define JUMP_TO_BOOT JUMP_TO_BOOT_SPECIFIC
#endif // __PLATFORM_H