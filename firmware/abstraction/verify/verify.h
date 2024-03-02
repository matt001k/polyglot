/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_ASYMMETRIC_H
#define __BL_ASYMMETRIC_H

/**************************************************************************//**
 * @file        verify.h
 *
 * @brief       Provides an abstraction layer for verify decryption used
 *              for validating firmware image signatures
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-24
 *****************************************************************************/
#include "config.h"

typedef BL_UINT8_T *(*Verify_Key_t)(void);
typedef BL_BOOL_T (*Verify_Cb_t)(BL_UINT8_T *hash,
                                 BL_UINT8_T *signature,
                                 BL_UINT8_T *key);

typedef struct
{
    BL_UINT8_T *key;
    struct
    {
        Verify_Key_t key;
        Verify_Cb_t verify;
    } cb;
} Verify_t;

/**************************************************************************//**
 * @brief Initialize The Configured Verification Module
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Verify_Init(void);

/**************************************************************************//**
 * @brief Get The Decryption Key
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Verify_GetKey(void);

/**************************************************************************//**
 * @brief Decrypt Block Of Data
 *
 * @details The API for getting the decryption key must be called before this.
 *
 * @param hash[in] calculated hash value
 * @param size[in] provided signature to validate
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t Verify_Decrypt(BL_UINT8_T *hash,
                        BL_UINT8_T *signature);


#endif //__BL_ASYMMETRIC_H
