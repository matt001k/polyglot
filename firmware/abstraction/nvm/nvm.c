/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup nvm
 * @{
 */

/**************************************************************************//**
 * @file        nvm.c
 *
 * @brief       Provides an abstraction layer for the bootloader's Non-Volatile
 *              memory storage. This can be anything from internal flash or
 *              external EEPROM/flash etc.
 * 
 * @see         config.h explains how to configure the NVM nodes
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-03
 *****************************************************************************/
#include "nvm.h"

#define NVM_NONE_OP (0U)
#define NVM_WRITE_OP (1U)
#define NVM_READ_OP (2U)
#define NVM_ERASE_OP (3U)

typedef struct
{
    NVM_Init_t init;        ///< Function pointer to initialization
    NVM_Write_t write;      ///< Function pointer to write to flash
    NVM_Read_t read;        ///< Function pointer to read from flash
    NVM_Erase_t erase;      ///< Function pointer to erase flash
    BL_UINT32_T size;       ///< Size of the partition to write to
    BL_UINT32_T offset;     ///< Offset of the partition to write to
    BL_UINT32_T page;       ///< Page size of the parition
    BL_UINT8_T priority;    ///< Priority of the partition to write to
    BL_UINT32_T p;          ///< Pointer to read and write locations
    BL_UINT8_T op;          ///< Operation that is currently ongoing
} nvm_Cfg_t;

typedef struct
{
    nvm_Cfg_t *cfg;    ///< Pointer to configuration
    BL_UINT8_T count;           ///< Number of NVM nodes
} nvm_t;

BL_STATIC nvm_Cfg_t nCfg[] =
{
#define NVM_ENTRY(init, write, read, erase, \
                  size, offset, page, priority) \
    {init, write, read, erase, size, offset, page, priority, offset},
    NVM_CFG
#undef NVM_ENTRY
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
};

BL_STATIC nvm_t nvm = {0U};

BL_Err_t NVM_Init(void)
{
    BL_Err_t err = BL_OK;
    nvm_Cfg_t temp = {0U};

    nvm.cfg = nCfg;

    /* Initialize all NVM nodes if available */
    while (nvm.cfg[nvm.count].init != 0 &&
           nvm.cfg[nvm.count].write != 0 &&
           nvm.cfg[nvm.count].read != 0 &&
           nvm.cfg[nvm.count].erase != 0)
    {
        if (nvm.cfg[nvm.count].init)
        {
            nvm.cfg[nvm.count].init();
        }
        nvm.count++;
    }

    /* Bubble sort ascending priorities */
    for (BL_UINT8_T i = 0U; i < nvm.count; i++)
    {
        for (BL_UINT8_T j = i + 1U; j < nvm.count; j++)
        {
            if (nCfg[i].priority > nCfg[j].priority)
            {
                temp = nCfg[i];
                nCfg[i] = nCfg[j];
                nCfg[j] = temp;
            }
        }
    }

    return err;
}

BL_Err_t NVM_Write(BL_UINT8_T node, BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = BL_EINVAL;

    if (node < nvm.count && length)
    {
        err = BL_ENODEV;
        if (nvm.cfg[node].write &&
            (nvm.cfg[node].op == NVM_NONE_OP ||
            nvm.cfg[node].op == NVM_WRITE_OP))
        {
            if ((nvm.cfg[node].p + length) >
                (nvm.cfg[node].offset + nvm.cfg[node].size))
            {
                length = nvm.cfg[node].offset +
                         nvm.cfg[node].size -
                         nvm.cfg[node].p;
            }

            if (length == 0U)
            {
                err = BL_ENOMEM;
            }
            else
            {
                err = BL_EALREADY;
                if (nvm.cfg[node].write(nvm.cfg[node].p, data, length) ==
                    BL_TRUE)
                {
                    nvm.cfg[node].op = NVM_WRITE_OP;
                    nvm.cfg[node].p += length;
                    err = BL_OK;
                }
            }
        }
    }

    return err;
}

BL_Err_t NVM_Read(BL_UINT8_T node, BL_UINT8_T *data, BL_UINT32_T *length)
{
    BL_Err_t err = BL_EINVAL;

    if (node < nvm.count && length)
    {
        err = BL_ENODEV;
        if (nvm.cfg[node].read &&
            (nvm.cfg[node].op == NVM_NONE_OP ||
            nvm.cfg[node].op == NVM_READ_OP))
        {
            if ((nvm.cfg[node].p + *length) >
                (nvm.cfg[node].offset + nvm.cfg[node].size))
            {
                *length = nvm.cfg[node].offset +
                         nvm.cfg[node].size -
                         nvm.cfg[node].p;
            }

            if (*length == 0U)
            {
                err = BL_ENOMEM;
            }
            else
            {
                err = BL_EALREADY;
                if (nvm.cfg[node].read(nvm.cfg[node].p, data, *length) ==
                    BL_TRUE)
                {
                    nvm.cfg[node].op = NVM_READ_OP;
                    nvm.cfg[node].p += *length;
                    err = BL_OK;
                }
            }
        }
    }

    return err;
}

BL_Err_t NVM_Erase(BL_UINT8_T node, BL_UINT32_T length)
{
    BL_Err_t err = BL_EINVAL;

    if (node < nvm.count && length)
    {
        err = BL_ENODEV;
        if (nvm.cfg[node].erase &&
            (nvm.cfg[node].op == NVM_NONE_OP ||
            nvm.cfg[node].op == NVM_ERASE_OP))
        {
            if ((nvm.cfg[node].p + length) <
                (nvm.cfg[node].offset + nvm.cfg[node].size))
            {
                length = nvm.cfg[node].offset +
                            nvm.cfg[node].size -
                            nvm.cfg[node].p;

            }

            if (length == 0U)
            {
                err = BL_ENOMEM;
            }
            else
            {
                err = BL_EALREADY;
                if (nvm.cfg[node].erase(nvm.cfg[node].p, length) ==
                    BL_TRUE)
                {
                    nvm.cfg[node].op = NVM_ERASE_OP;
                    nvm.cfg[node].p += length;
                    err = BL_OK;
                }
            }

        }
    }

    return err;
}

BL_Err_t NVM_OperationFinish(BL_UINT8_T node)
{
    BL_Err_t err = BL_EINVAL;

    if (node < nvm.count)
    {
        err = BL_OK;
        if (nvm.cfg[node].p != nvm.cfg[node].offset)
        {
            nvm.cfg[node].p = nvm.cfg[node].offset;
        }
        nvm.cfg[node].op = NVM_NONE_OP;
    }

    return err;
}

BL_Err_t NVM_GetSize(BL_UINT8_T node, BL_UINT32_T *size)
{
    BL_Err_t err = BL_EINVAL;

    if (node < nvm.count && size)
    {
        err = BL_OK;
        *size = nvm.cfg[node].size;
    }

    return err;
}

BL_Err_t NVM_GetPageSize(BL_UINT8_T node, BL_UINT32_T *size)
{
    BL_Err_t err = BL_EINVAL;

    if (node < nvm.count && size)
    {
        err = BL_OK;
        *size = nvm.cfg[node].page;
    }

    return err;
}

BL_Err_t NVM_GetCount(BL_UINT8_T *count)
{
    BL_Err_t err = BL_EINVAL;

    if (count)
    {
        *count = nvm.count;
        err = BL_OK;
    }

    return err;
}

BL_Err_t NVM_GetLocation(BL_UINT8_T node, BL_UINT32_T *location)
{
    BL_Err_t err = BL_EINVAL;
    
    if (node < nvm.count && location)
    {
        err = BL_OK;
        *location = nvm.cfg[node].offset;
    }

    return err;
}

/**@} nvm */
