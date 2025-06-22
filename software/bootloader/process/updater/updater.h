/******************************************************************************
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_UPDATER_H
#define __BL_UPDATER_H

/**
 * @addtogroup updater
 * @{
 */

/******************************************************************************
 * @file        command.h
 *
 * @brief       Provides function necessary to update the device
 *
 * @author      Matthew Krause
 *
 * @date        2024-03-30
 *****************************************************************************/
#include <iostream>
#include <vector>
#include "serial.h"
#include "common.h"

class Updater
{
    public:
        Updater(Serial serial);
        ~Updater();
        BL_Err_t Update(std::vector<uint8_t> data, std::size_t transfer);
    private:
        Serial m_serial;
};

/**@} updater */

#endif // __BL_UPDATER_H
