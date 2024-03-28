/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_UPDATER_H
#define __BL_UPDATER_H

/**
 * @addtogroup updater
 * @{
 */

/**************************************************************************//**
 * @file        updater.h
 *
 * @brief       Provides an interface to update partitions on the device.
 *              This is meant to be used in the callbacks necessary for
 *              incoming data to the device and follows the requirements
 *              necessary for the data module's callback functions.
 *
 * @author      Matthew Krause
 *
 * @date        2024-03-18
 *****************************************************************************/
#include "config.h"

BL_Err_t Updater_Start(void);
BL_Err_t Updater_Handle(BL_UINT8_T *buf, BL_UINT32_T size);


/**@} updater */

#endif // __BL_UPDATER_H
