/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup data
 * @{
 */

/**************************************************************************//**
 * @file        data.cpp
 *
 * @brief       Provides and interface for the necessary data needed for
 *              the bootloader
 *
 * @author      Matthew Krause
 *
 * @date        2022-10-10
 *****************************************************************************/
#include "data.h"

Data::Data()
{

}

Data::~Data()
{

}

BL_Err_t Data::Send_Length(Serial serial,
                           uint32_t length)
{
    uint8_t buf[sizeof(uint32_t)] = {0U};
    int8_t bIdx = sizeof(uint32_t) - 1U;

    buf[bIdx--] = (uint8_t) (length);
    for (; bIdx >= 0; --bIdx)
    {
        buf[bIdx] = (uint8_t) (length >>= 8U);
    }

    return serial.Transmit(buf, sizeof(uint32_t));
}

BL_Err_t Data::Send_Data(Serial serial,
                         uint8_t *data,
                         uint32_t length)
{
    return serial.Transmit(data, length);
}

/**@} data */
