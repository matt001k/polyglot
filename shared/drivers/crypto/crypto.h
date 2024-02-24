/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup crypto
 * @{
 */

/**
 * @file        crypto.h
 *
 * @brief       Provides the API AES-CBC Decryption
 *
 * @author      Matthew Krause
 *
 * @date        2024-02-11
 */

/*****************************************************************************/
#ifndef __CRYPTO_H
#define __CRYPTO_H

#include <stdint.h>
#include <stdbool.h>
#include "platform.h"

void Crypto_AESInit(void);
bool Crypto_AESDecrypt(uint8_t *input,
                       uint8_t *output,
                       uint32_t size,
                       uint8_t *key,
                       uint8_t *iv);
void Crypto_SHA256Start(void);
bool Crypto_SHA256Update(uint8_t *data, uint32_t size);
bool Crypto_SHA256Finish(uint8_t *digest);


#endif //__CRYPTO_H
