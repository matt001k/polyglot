/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_ECC_H
#define __BL_ECC_H

/**************************************************************************//**
 * @file        ecc.h
 *
 * @brief       Provides an abstraction layer for verify decryption used
 *              for validating firmware image signatures
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-24
 *****************************************************************************/
#include "config.h"

#define ECC_SIGNATURE_LENGTH 64

typedef BL_UINT8_T *(*ECC_Key_t)(void);
typedef BL_BOOL_T (*ECC_Cb_t)(BL_UINT8_T *hash,
                                 BL_UINT8_T *signature,
                                 BL_UINT8_T *key);

typedef struct
{
    BL_UINT8_T *key;
    struct
    {
        ECC_Key_t key;
        ECC_Cb_t verify;
    } cb;
} ECC_t;

/**************************************************************************//**
 * @brief Initialize The Configured ECC Verification Module
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t ECC_Init(void);

/**************************************************************************//**
 * @brief Get The ECC Decryption Key
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t ECC_GetKey(void);

/**************************************************************************//**
 * @brief Decrypt Signature And Compare To Hash
 *
 * @details The API for getting the decryption key must be called before this.
 *
 * @param hash[in] calculated hash value
 * @param signature[in] provided signature to validate
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t ECC_Decrypt(BL_UINT8_T *hash,
                     BL_UINT8_T *signature);


#endif //__BL_ASYMMETRIC_H
