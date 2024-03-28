/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_DATA_H
#define __BL_DATA_H

/**
 * @addtogroup data
 * @{
 */

/**************************************************************************//**
 * @file        data.h
 *
 * @brief       Provides an interface for the bootloader to send commands to
 *              and receive commands from the device loading the new
 *              application
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "config.h"

typedef BL_UINT32_T DataLength_t;

/**************************************************************************//**
 * @brief Initialize the data interface length callback
 *
 * @details This implements a callback to determine data length from the serial
 *          peripheral
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Data_LengthCbInit(void);

/**************************************************************************//**
 * @brief Deinitialize the data interface length callback
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Data_LengthCbDeinit(void);

/**************************************************************************//**
 * @brief Initialize the data interface data callback
 *
 * @details This implements a callback to determine data length from the serial
 *          peripheral
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Data_DataCbInit(void);

/**************************************************************************//**
 * @brief Deinitialize the data interface data callback
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Data_DataCbDeinit(void);

/**************************************************************************//**
 * @brief Resets the data interface
 *
 * @details Resets static variables in the data interface
 *
 * @return BL_Err_t
 *****************************************************************************/
void Data_Reset(void);

/**************************************************************************//**
 * @brief Get the length of the next incoming data stream
 *
 * @details This API must be called before Data_ReceiveData and Data_SetLength
 *          to get the length of the datastream from the serial buffer
 *
 * @param length[out] length of the data to be
 * @return BL_Err_t BL_ENODATA when no data is available
 *****************************************************************************/
BL_Err_t Data_GetLength(DataLength_t *length);

/**************************************************************************//**
 * @brief Set the length of the next incoming data stream
 *
 * @details This API must be called before Data_ReceiveData to receive the
 *          the correct amount over the data stream
 *
 * @param length[in] the length of the data to set for the data stream
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Data_SetLength(DataLength_t length);

/**************************************************************************//**
 * @brief Receives to a buffer
 *
 * @param buf[in/out] buffer to receive data to
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Data_ReceiveData(BL_UINT8_T *buf);

/**@} data */

#endif //__BL_DATA_H
