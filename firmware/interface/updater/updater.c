/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**************************************************************************//**
 * @file        updater.c
 *
 * @brief       Provides an interface to update partitions on the device.
 *              This is meant to be used in the callbacks necessary for
 *              incoming data to the device and follows the requirements
 *              necessary for the update callback functions.
 *
 * @author      Matthew Krause
 *
 * @date        2024-03-18
 *****************************************************************************/
#include "updater.h"
#include "writer.h"

BL_Err_t Updater_Start(void)
{
    BL_Err_t err = BL_ERR;
    if ((err = Writer_Start()) == BL_EALREADY)
    {
        err = BL_OK;
    }
    else if (err == BL_EALREADY)
    {
        err = BL_OK;
    }
    return err;
}

BL_Err_t Updater_Handle(BL_UINT8_T *buf, BL_UINT32_T size)
{
    BL_Err_t err = BL_ERR;
    err = Writer_WriteData(buf, size);
    return err;
}

