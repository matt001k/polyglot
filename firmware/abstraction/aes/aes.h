/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_AES_H
#define __BL_AES_H

/**
 * @addtogroup aes
 * @{
 */

/**************************************************************************//**
 * @file        aes.h
 *
 * @brief       Provides an abstraction layer for the AES decryption methods
 *              necessary for ingesting images over serial communication and
 *              ensuring safe application loading.
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-11
 *****************************************************************************/
#include "config.h"

#define AES_128_IV_SIZE 16
#define AES_128_KEY_SIZE 16

typedef void (*AES_Init_t)(void);
typedef BL_BOOL_T (*AES_Decrypt_t)(BL_UINT8_T *input,
                                   BL_UINT8_T *output,
                                   BL_UINT32_T size,
                                   uint8_t *key,
                                   uint8_t *iv);
typedef struct
{
    struct
    {
        BL_UINT8_T *input;
        BL_UINT8_T *output;
    } buf;
    BL_UINT8_T *iv;
    BL_UINT8_T *key;
    struct
    {
        AES_Init_t init;
        AES_Decrypt_t decrypt;
    } cb;
} AES_t;


/**************************************************************************//**
 * @brief Initialize The Configured AES Peripheral
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t AES_Init(void);

/**************************************************************************//**
 * @brief Set The Decryption Key
 *
 * @param key[in] pointer to key to be utilized for decryption, must be sized
 *                AES_128_KEY_SIZE
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t AES_SetKey(BL_UINT8_T *key);

/**************************************************************************//**
 * @brief Set The Decryption Initialization Vector
 *
 * @param key[in] pointer to initialization vector to be utilized for
 *                decryption, must be sized AES_128_IV_SIZE
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t AES_SetIV(BL_UINT8_T *iv);

/**************************************************************************//**
 * @brief Decrypt Block Of Data
 *
 * @details The API for setting the decryption key and initialization vector
 *          must be called before this API.
 *
 * @param input[in] data buffer to decrypt
 * @param output[out] output buffer for decrypted data
 * @param size[in] size of data to decrypt, must be bound by AES_128_IV_SIZE
 *                 bytes
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t AES_Decrypt(BL_UINT8_T *input, BL_UINT8_T *output, BL_UINT32_T size);


#endif //__BL_AES_H
