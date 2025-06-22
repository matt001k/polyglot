/******************************************************************************
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_WRITER_H
#define __BL_WRITER_H

/**
 * @addtogroup writer
 * @{
 */

/******************************************************************************
 * @file        writer.h
 *
 * @brief       Provides an interface to write/read from partitions.
 *              This API has the ability to update and interact with the
 *              partition table API.
 *
 * @author      Matthew Krause
 *
 * @date        2024-03-10
 *****************************************************************************/
#include "config.h"

BL_Err_t Writer_Start(void);
BL_Err_t Writer_WriteData(BL_UINT8_T *data, BL_UINT32_T length);
BL_Err_t Writer_ReadData(BL_UINT8_T *data, BL_UINT32_T *length);
BL_Err_t Writer_Finish(void);
void Writer_States(BL_Err_t **err, BL_UINT8_T *count);

/**@} writer */

#endif // __BL_WRITER_H
