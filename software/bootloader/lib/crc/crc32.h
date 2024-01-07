/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_CRC32_H
#define __BL_CRC32_H

/**
 * @addtogroup crc32
 * @{
 */

/**************************************************************************//**
 * @file        crc32.h
 *
 * @brief       Provides an interface for calculating 32 bit CRC of data
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-22
 *****************************************************************************/
#include <iostream>

std::uint32_t CRC32(std::uint32_t crc,
                    const void *buf,
                    std::uint32_t size);

/**@} crc32 */

#endif //__BL_CRC32_H