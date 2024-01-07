/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_VALIDATOR_H
#define __BL_VALIDATOR_H

/**
 * @addtogroup validator
 * @{
 */

/**************************************************************************//**
 * @file        validator.h
 *
 * @brief       Provides an interface to validate what is in the application
 *              partition to be able to run the application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-30
 *****************************************************************************/
#include "config.h"

BL_Err_t Validator_Run(BL_UINT8_T *buf, BL_UINT32_T size);


/**@} validator */

#endif //__BL_VALIDATOR_H