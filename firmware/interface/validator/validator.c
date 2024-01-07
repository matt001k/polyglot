/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

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
#include "validator.h"
#include "dict.h"
#include "nvm.h"
#include "helper.h"
#include "crc32.h"

#define ENDING_PHRASE_LENGTH (SECRET_KEY_SIZE + CRC32_SIZE)
#define CRC_CALCULATED (0U)
#define CRC_FLASH (1U)
#define NUM_CRC_BUFFER (2U)


BL_Err_t Validator_Run(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = BL_OK;
    BL_CONST BL_UINT32_T secret = SECRET_KEY_WORD;
    BL_UINT8_T bSecret[SECRET_KEY_SIZE] = {0U};
    BL_BOOL_T done = BL_FALSE;
    BL_UINT32_T tSize = length / 2U;
    BL_UINT32_T lSize = tSize;
    BL_UINT8_T *l = data + lSize;
    BL_UINT32_T crc[NUM_CRC_BUFFER] = {0U};
    BL_UINT8_T offset = 0U;
    BL_UINT8_T found = 0U;
    BL_BOOL_T lFilled = BL_TRUE;
    UINT32_UINT8(bSecret, secret);

    while (done == BL_FALSE && err == BL_OK)
    {
        /* Read the partition at the split buffer (look behind) */
        while ((err = NVM_Read(APPLICATION_NODE, l, &lSize)) ==
                BL_EALREADY) {};
        
        /* Look for secret word in */
        for (BL_UINT32_T pIdx = 0U;
                         pIdx < lSize;
                         pIdx++)
        {
            for (BL_UINT8_T sIdx = 0U;
                    sIdx < SECRET_KEY_SIZE && (pIdx + sIdx) < lSize;
                    sIdx++)
            {
                if (l[pIdx + sIdx] == bSecret[sIdx + offset])
                {
                    found++;
                    if ((sIdx + offset) == 
                        (SECRET_KEY_SIZE - 1U))
                    {
                        done = BL_TRUE;
                        tSize = pIdx + sIdx + 1U;
                        break;
                    }
                }
                else
                {
                    found = 0U;
                    break;
                }
            }
            offset = found;
            if (found == SECRET_KEY_SIZE)
            {
                break;
            }
        }
        if (lFilled == BL_TRUE)
        {
            lFilled = BL_FALSE;
            MEMCPY(data, l, lSize);
        }
        else if (found == BL_FALSE)
        {
            crc[CRC_CALCULATED] = CRC32(crc[CRC_CALCULATED], data, lSize);
            MEMCPY(data, l, lSize);
        }
        else
        {
            /* Rare case where binary data sent is not aligned to look-ahead
             * buffer boundry */
            if (tSize < ENDING_PHRASE_LENGTH)
            {
                crc[CRC_CALCULATED] = CRC32(crc[CRC_CALCULATED],
                                            data,
                                            lSize - 
                                            (ENDING_PHRASE_LENGTH -
                                            tSize));
                BL_UINT8_T cIdx = 0U;
                for (; cIdx < (ENDING_PHRASE_LENGTH - tSize); cIdx++)
                {
                    bSecret[cIdx] = data[lSize -
                                            (ENDING_PHRASE_LENGTH - tSize)
                                            + cIdx];
                }
                for (BL_UINT8_T cJdx = 0U; (cJdx + cIdx) < CRC32_SIZE; cJdx++)
                {
                    bSecret[cJdx + cIdx] = l[cJdx];
                }
                UINT8_UINT32(&crc[CRC_FLASH], bSecret);
            }
            else
            {
                crc[CRC_CALCULATED] = CRC32(crc[CRC_CALCULATED], data, lSize);
                MEMCPY(data, l, tSize);
                crc[CRC_CALCULATED] = CRC32(crc[CRC_CALCULATED],
                                            data,
                                            tSize - ENDING_PHRASE_LENGTH);
                UINT8_UINT32(&crc[CRC_FLASH], &l[tSize - ENDING_PHRASE_LENGTH]);
            }
        }
    }
    err = NVM_OperationFinish(APPLICATION_NODE);
    if (err == BL_OK)
    {
        if (crc[CRC_CALCULATED] != crc[CRC_FLASH])
        {
            err = BL_ERR;
        } 
    }

    return err;
}
