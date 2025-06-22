/******************************************************************************
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_DATA_H
#define __BL_DATA_H

/**
 * @addtogroup data
 * @{
 */

/******************************************************************************
 * @file        data.h
 *
 * @brief       Provides and interface for the necessary data needed for
 *              the bootloader
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-01
 *****************************************************************************/
#include <iostream>
#include "common.h"
#include "serial.h"
#include <stdint.h>

class Data
{
public:
    typedef enum
    {
        DATA_LENGTH,
        DATA,
        DATA_NUM,
    } Data_Type_e;
    Data();
    ~Data();
    BL_Err_t Send_Length(Serial serial, uint32_t length);
    BL_Err_t Send_Data(Serial serial, uint8_t *data, uint32_t length);
private:
};

/**@} data */

#endif // __BL_DATA_H
