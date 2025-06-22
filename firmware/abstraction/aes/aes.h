/******************************************************************************
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_AES_H
#define __BL_AES_H

/**
 * @addtogroup aes
 * @{
 */

/******************************************************************************
 * @file        aes.h
 *
 * @brief       Provides an abstraction layer for the AES decryption methods
 *              necessary for ingesting images over serial communication and
 *              ensuring safe application loading. This shall use
 *              CBC encryption.
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-11
 *****************************************************************************/
#include "config.h"

#define AES_IV_SIZE      16
#define AES_128_KEY_SIZE 16
#define AES_KEY_SIZE     AES_128_KEY_SIZE

/******************************************************************************
 * @brief Initialize The Configured AES Peripheral
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t AES_Init(void);

/******************************************************************************
 * @brief Set The Decryption Key
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t AES_SetKey(void);

/******************************************************************************
 * @brief Set The Decryption Initialization Vector
 *
 * @param iv[in] initialization vector for AES operations
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t AES_SetIV(uint8_t *iv);

/******************************************************************************
 * @brief Decrypt Block Of Data
 *
 * @details The API for setting the decryption key and initialization vector
 *          must be called before this API.
 *
 * @param input[in] data buffer to decrypt
 * @param output[out] output buffer for decrypted data
 * @param size[in] size of data to decrypt, must be bound by AES_IV_SIZE
 *                 bytes
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t AES_Decrypt(BL_UINT8_T *input, BL_UINT8_T *output, BL_UINT32_T size);

#endif  //__BL_AES_H
