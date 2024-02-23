/**************************************************************************//**
 * (c) 2024 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_SHA256_H
#define __BL_SHA256_H

/**************************************************************************//**
 * @file        sha256.h
 *
 * @brief       Provides an abstraction layer for SHA256 hashing functions
 *              to validate signatures of incoming firmware images
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-18
 *****************************************************************************/
#include "config.h"

typedef void (*SHA_Start_t)(void);
typedef BL_BOOL_T (*SHA_Update_t)(BL_UINT8_T *data, BL_UINT32_T size);
typedef BL_BOOL_T (*SHA_Finish_t)(BL_UINT8_T *digest);
typedef struct
{
    BL_BOOL_T started;
    struct
    {
        SHA_Start_t start;
        SHA_Update_t update;
        SHA_Finish_t finish;
    } cb;
} SHA_t;

/**************************************************************************//**
 * @brief Initialize The Configured SHA256 Peripheral
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t SHA256_Init(void);

/**************************************************************************//**
 * @brief Calculate The SHA256 For Chunk Of Data
 *
 * Calculates the SHA256 checksum for chunk of data. Can be called iteratively.
 * Must be ran befor SHA256_Finish.
 *
 * @param data[in] data to run SHA256 hashing function to
 * @param size[in] size of data to run SHA256 hashing function to
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t SHA256_Calculate(BL_UINT8_T *data,
                          BL_UINT32_T size);

/**************************************************************************//**
 * @brief Finalizes SHA256 Hashing
 *
 * Finalizes SHA256 hashing and outputs the calculated digest.
 *
 * @param digest[out] digest output
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t SHA256_Finish(BL_UINT8_T *digest);

#endif//__BL_SHA256_H
