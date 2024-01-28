/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup loader
 * @{
 */

/**************************************************************************//**
 * @file        loader.c
 *
 * @brief       Provides an interface to write the application to the main
 *              partition as well as the oldest backup partition/specified
 *              partition
 * 
 * @author      Matthew Krause
 *
 * @date        2022-10-05
 *****************************************************************************/
#include "loader.h"
#include "dict.h"
#include "nvm.h"
#include "helper.h"
#include "crc32.h"

#define LOADER_PARTITION_REV_SIZE (BL_SIZEOF(BL_UINT32_T))
#define LOADER_PARTITION_INIT_REV (0xFFFFFFFFU)
#define LOADER_PARTITION_GET_SIZE(c) ((c * LOADER_PARTITION_REV_SIZE) + \
                                      CRC32_SIZE)

typedef enum
{
    INIT_CHECK = 0U,
    INIT_ERASE,
    INIT_CREATE,
    INIT_PREPARE,
} loader_Init_States_e;

typedef enum
{
    COUNT_PARTITIONS = 0U,
    GET_PARTITIONS,
    CHECK_PARTITIONS,
    SIZE_PARTITIONS,
    ERASE_PARTITIONS,
} loader_Prepare_States_e;

typedef enum
{
    OBTAIN_PARTITION_INFO = 0,
    ERASE_PARTITION_NODE,
    WRITE_REVISIONS,
    CALCULATE_PARITION_CRC,
    VALIDATE_PARTITION_CRC,
} loader_Revision_States_e;

typedef struct
{
    BL_UINT8_T node;
    BL_UINT32_T revision;
    BL_UINT32_T size;
    BL_UINT32_T length;
    BL_UINT32_T crc;
} loader_Info_t;

BL_STATIC loader_Info_t partitions[BL_NUM_PARTITIONS_TO_UPDATE] = {0U};

BL_STATIC BL_Err_t loader_CheckPartition(BL_BOOL_T *check, BL_BOOL_T *reset);
BL_STATIC BL_Err_t loader_ErasePartition(void);
BL_STATIC BL_Err_t loader_CreatePartition(void);
BL_STATIC BL_Err_t loader_PreparePartitions(BL_UINT8_T *buf, BL_UINT32_T size);

BL_Err_t Loader_Init(BL_UINT8_T *buf, BL_UINT32_T size)
{
    BL_Err_t err = BL_ERR;
    BL_STATIC loader_Init_States_e state = INIT_CHECK;
    BL_STATIC BL_BOOL_T check = BL_FALSE;
    BL_STATIC BL_BOOL_T reset = BL_FALSE;

    switch (state)
    {
    case INIT_CHECK:
        MEMSET(partitions, 0U, BL_SIZEOF(partitions));
        if (loader_CheckPartition(&check, &reset) == BL_OK)
        {
            if (!check)
            {
                state = INIT_ERASE;
            }
            else
            {
                state = INIT_PREPARE;
            }
        }
        break;
    case INIT_ERASE:
        if (loader_ErasePartition() == BL_OK)
        {
            state = INIT_CREATE;
        }
        else
        {
            break;
        }
    case INIT_CREATE:
        if (loader_CreatePartition() == BL_OK)
        {
            reset = BL_TRUE;
            state = INIT_CHECK;
        }
        break;
    case INIT_PREPARE:
        if ((err = loader_PreparePartitions(buf, size)) == BL_OK)
        {
            state = INIT_CHECK;
        }
        break;
    default:
        break;
    }

    return err;
}

BL_Err_t Loader_Write(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = BL_EINVAL;
    BL_STATIC BL_BOOL_T done[BL_NUM_PARTITIONS_TO_UPDATE] = {BL_FALSE};
    BL_UINT8_T wIdx = 0U;

    if (data && length)
    {
        for (; wIdx < BL_NUM_PARTITIONS_TO_UPDATE; wIdx++)
        {
            if (done[wIdx] == BL_FALSE)
            {
                if ((err = NVM_Write(partitions[wIdx].node, data, length)) ==
                    BL_EINVAL)
                {
                    done[wIdx] = BL_TRUE;
                }
                else if (err == BL_OK)
                {
                    partitions[wIdx].length += length;
                    done[wIdx] = BL_TRUE;
                }
                else
                {
                    break;
                }
            }
        }

        err = BL_EALREADY;
        if (wIdx == BL_NUM_PARTITIONS_TO_UPDATE)
        {
            for (wIdx = 0U; wIdx < BL_NUM_PARTITIONS_TO_UPDATE; wIdx++)
            {
                done[wIdx] = BL_FALSE;
            }
            err = BL_OK;
        }
    }

    return err;
}

BL_Err_t Loader_WriteSecret(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = BL_ERR;
    BL_STATIC BL_CONST BL_UINT32_T secret = SECRET_KEY_WORD;
    BL_STATIC BL_BOOL_T done[BL_NUM_PARTITIONS_TO_UPDATE] = {BL_FALSE};
    BL_UINT8_T sIdx = 0U;

    /* Pack secret word onto end of partitions */
    MEMSET(data, 0U, length);
    UINT32_UINT8(data, secret);
    for (; sIdx < BL_NUM_PARTITIONS_TO_UPDATE; sIdx++)
    {
        if (done[sIdx] == BL_FALSE)
        {
            if ((err = NVM_Write(partitions[sIdx].node,
                                    data,
                                    SECRET_KEY_SIZE)) ==
                BL_EINVAL || err == BL_OK)
            {
                done[sIdx] = BL_TRUE;
            }
            else
            {
                break;
            }
        }
    }
    err = (err == BL_OK || err == BL_EALREADY) ? BL_EALREADY : BL_ERR;
    if (sIdx == BL_NUM_PARTITIONS_TO_UPDATE || err == BL_ERR)
    {
        err = (err == BL_ERR) ? BL_ERR : BL_OK;
        for (sIdx = 0U; sIdx < BL_NUM_PARTITIONS_TO_UPDATE; sIdx++)
        {
            done[sIdx] = BL_FALSE;
        }
    }

    return err;
}

BL_Err_t Loader_Reset(void)
{
    BL_Err_t err = BL_OK;

    for (BL_UINT8_T wIdx = 0U; wIdx < BL_NUM_PARTITIONS_TO_UPDATE; wIdx++)
    {
        if ((err = NVM_OperationFinish(partitions[wIdx].node)) != BL_OK)
        {
            break;
        }
    }

    return err;
}

BL_Err_t Loader_Validate(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = BL_ERR;
    BL_STATIC BL_BOOL_T done[BL_NUM_PARTITIONS_TO_UPDATE] = {BL_FALSE};
    BL_STATIC BL_BOOL_T match[BL_NUM_PARTITIONS_TO_UPDATE] = {BL_FALSE};
    BL_UINT32_T l_temp = 0U;
    BL_UINT8_T vIdx = 0U;

    if (data && length)
    {
        for (; vIdx < BL_NUM_PARTITIONS_TO_UPDATE; vIdx++)
        {
            if (done[vIdx] == BL_FALSE)
            {
                if (partitions[vIdx].length == CRC32_SIZE)
                {
                    l_temp = partitions[vIdx].length;
                    if ((err = NVM_Read((partitions[vIdx].node),
                                        data,
                                        &l_temp)) != BL_OK)
                    {
                        break;
                    }
                    else
                    {
                        partitions[vIdx].length -= CRC32_SIZE;
                        done[vIdx] = BL_TRUE;
                        /**
                         * Use temp variable to hold the CRC32 and compare to
                         * calculated CRC32 to determine if it was stored
                         * correct
                         */
                        l_temp = (BL_UINT32_T) (data[0] << 24U |
                                                data[1] << 16U |
                                                data[2] << 8U |
                                                data[3]);
                        if (partitions[vIdx].crc == l_temp)
                        {
                            match[vIdx] = BL_TRUE;
                        }
                    }
                }
                else
                {
                    l_temp = length < (partitions[vIdx].length - CRC32_SIZE) ?
                             length : (partitions[vIdx].length - CRC32_SIZE);
                    if ((err = NVM_Read((partitions[vIdx].node),
                                        data,
                                        &l_temp)) == BL_OK)
                    {
                        partitions[vIdx].crc = CRC32(partitions[vIdx].crc,
                                                        data,
                                                        l_temp);
                        partitions[vIdx].length -= l_temp;
                    }
                    break;
                }
            }
        }
    }

    /* Determine if all partitions have been validated */
    err = BL_EALREADY;
    for (vIdx = 0U; vIdx < BL_NUM_PARTITIONS_TO_UPDATE; vIdx++)
    {
        if (done[vIdx] == BL_FALSE)
        {
            break;
        }
    }
    if (vIdx == BL_NUM_PARTITIONS_TO_UPDATE)
    {
        err = BL_OK;
        for (vIdx = 0U; vIdx < BL_NUM_PARTITIONS_TO_UPDATE; vIdx++)
        {
            done[vIdx] = BL_FALSE;
            if (match[vIdx] == BL_FALSE)
            {
                err = BL_ERR;
                break;
            }
        }
    }

    return err;
}

BL_Err_t Loader_UpdateRevisions(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = BL_ERR;
    BL_STATIC loader_Revision_States_e state = OBTAIN_PARTITION_INFO;
    BL_STATIC BL_UINT32_T lTemp = 0U;
    BL_STATIC BL_UINT32_T lCrc = 0U;
    BL_STATIC BL_UINT8_T count = 0U;
    BL_UINT32_T cCrc = 0U;
    BL_UINT8_T *p = BL_NULL;
    BL_UINT32_T rTemp = 0U;

    if (data && length)
    {
        err = BL_EALREADY;
        switch (state)
        {
        case OBTAIN_PARTITION_INFO:
            /* Read the partition information */
            if (NVM_Read(PARTITION_NODE, data, &length) == BL_OK)
            {
                NVM_OperationFinish(PARTITION_NODE);
                /* Update the new partition revisions into the working buf */
                for (BL_UINT8_T i = 0U;
                        i < BL_NUM_PARTITIONS_TO_UPDATE;
                        i++)
                {
                    partitions[i].revision--;
                    rTemp = partitions[i].revision;
                    p = data +
                        (partitions[i].node * LOADER_PARTITION_REV_SIZE) +
                        LOADER_PARTITION_REV_SIZE - 1U;
                    *p-- = (BL_UINT8_T) (rTemp);
                    for (; p >= data +
                                (partitions[i].node *
                                LOADER_PARTITION_REV_SIZE);
                            p--)
                    {
                        *p = (BL_UINT8_T) (rTemp >>= 8U);
                    }
                }
                state = ERASE_PARTITION_NODE;
            }
            else
            {
                break;
            }
        case ERASE_PARTITION_NODE:
            /* Erase the loader partition node */
            if (NVM_GetSize(PARTITION_NODE, &lTemp) == BL_OK)
            {
                if (NVM_Erase(PARTITION_NODE, lTemp) == BL_OK)
                {
                    NVM_OperationFinish(PARTITION_NODE);
                    state = CALCULATE_PARITION_CRC;
                }
                else
                {
                    break;
                }
            }
        case CALCULATE_PARITION_CRC:
            if (NVM_GetCount(&count) == BL_OK)
            {
                /* Ensure the buffer can hold the required size of the table */
                if (length < LOADER_PARTITION_GET_SIZE(count))
                {
                    err = BL_ENOMEM;
                    break;
                }
                else
                {
                    rTemp = LOADER_PARTITION_GET_SIZE(count) - CRC32_SIZE;
                    lCrc = CRC32(lCrc,
                                 data,
                                 rTemp);
                    UINT32_UINT8(&data[rTemp], lCrc);
                    state = WRITE_REVISIONS;
                }
            }
            /* Intentional fallthrough on state change */
        case WRITE_REVISIONS:
            /* Write the updated buffer to the loader partition node */
            if (NVM_Write(PARTITION_NODE, data, length) == BL_OK)
            {
                NVM_OperationFinish(PARTITION_NODE);
                MEMSET(data, 0U, length);
                state = VALIDATE_PARTITION_CRC;

            }
            else
            {
                break;
            }
        case VALIDATE_PARTITION_CRC:
            /* Read the partition */
            rTemp = LOADER_PARTITION_GET_SIZE(count);
            if (NVM_Read(PARTITION_NODE,
                         data,
                         &rTemp) == BL_OK)
            {
                NVM_OperationFinish(PARTITION_NODE);
                UINT8_UINT32(&cCrc, &data[rTemp - CRC32_SIZE]);
                if (lCrc == cCrc)
                {
                    err = BL_OK;
                }
                else
                {
                    err = BL_ERR;
                }
                state = OBTAIN_PARTITION_INFO;
                lTemp = lCrc = count = 0U;
            }
            else
            {
                break;
            }
        default:
            break;
        }
    }

    return err;
}

BL_Err_t Loader_SetCustomNodes(BL_UINT8_T *nodes, BL_UINT8_T count)
{
    BL_Err_t err = BL_ERR;

    if (nodes && count)
    {
        err = BL_OK;
        MEMSET(partitions, 0U, BL_SIZEOF(partitions));
        for (BL_UINT8_T i = 0U; i < count; i++)
        {
            partitions[i].node = nodes[i];
            err = NVM_GetSize(partitions[i].node, &partitions[i].size);
        }
    }
    
    return err;
}

BL_STATIC BL_Err_t loader_CheckPartition(BL_BOOL_T *check, BL_BOOL_T *reset)
{
    BL_STATIC BL_Err_t err = BL_ERR;
    BL_UINT8_T buf[SECRET_KEY_SIZE] = {0U};
    BL_STATIC BL_CONST BL_UINT32_T secret = SECRET_KEY_WORD;
    BL_UINT8_T i = 0U;
    BL_UINT32_T size = SECRET_KEY_SIZE;

    if (*reset == BL_TRUE)
    {
        *reset = BL_FALSE;
    }
    err = NVM_Read(PARTITION_NODE, buf, &size);
    if (err == BL_OK)
    {
        /* Check for secret word in partition */
        for (; i < SECRET_KEY_SIZE; i++)
        {
            if (buf[i] != (BL_UINT8_T) (secret >>
                          ((SECRET_KEY_SIZE * 8U) -
                          (8U * (i + 1U)))))
            {
                *check = false;
                break;
            }
        }
        if (i == SECRET_KEY_SIZE)
        {
            *check = true;
        }
        NVM_OperationFinish(PARTITION_NODE);
    }

    return err;
}

BL_STATIC BL_Err_t loader_ErasePartition(void)
{
    BL_Err_t err = BL_ERR;
    BL_STATIC BL_BOOL_T op = BL_FALSE;
    BL_STATIC BL_UINT32_T size = 0U;
    
    if (op == BL_FALSE)
    {
        if (NVM_GetSize(PARTITION_NODE, &size) == BL_OK)
        {
            op = BL_TRUE;
        }
    }
    /* Erase partition table non-blocking */
    if ((err = NVM_Erase(PARTITION_NODE, size)) == BL_OK)
    {
        op = BL_FALSE;
        size = 0U;
        NVM_OperationFinish(PARTITION_NODE);
    }

    return err;
}

BL_STATIC BL_Err_t loader_CreatePartition(void)
{
    BL_Err_t err = BL_ERR;
    BL_STATIC BL_BOOL_T op = BL_FALSE;
    BL_STATIC BL_UINT8_T buf[SECRET_KEY_SIZE] = {0U};
    BL_UINT32_T secret = SECRET_KEY_WORD;

    /* Convert secret into byte buffer */
    if (op == BL_FALSE)
    {
        UINT32_UINT8(buf, secret);
        op = BL_TRUE;
    }

    /* Write to partition table non-blocking */
    if ((err = NVM_Write(PARTITION_NODE,
                         buf,
                         SECRET_KEY_SIZE)) == BL_OK)
    {
        op = BL_FALSE;
        MEMSET(buf, 0U, SECRET_KEY_SIZE);
        NVM_OperationFinish(PARTITION_NODE);
    }

    return err;
}

BL_STATIC BL_Err_t loader_PreparePartitions(BL_UINT8_T *buf, BL_UINT32_T size)
{
    BL_Err_t err = BL_ERR;
    BL_STATIC loader_Prepare_States_e state = COUNT_PARTITIONS;
    BL_STATIC BL_UINT8_T count = 0U;
    BL_STATIC BL_UINT8_T erase = 0U;
    BL_UINT32_T info = 0U;
    BL_UINT8_T offset = 0;

    switch (state)
    {
    case COUNT_PARTITIONS:
        if (NVM_GetCount(&count) == BL_OK)
        {
            /* Ensure the buffer can hold the required size of the table */
            if (size < LOADER_PARTITION_GET_SIZE(count))
            {
                err = BL_ENOMEM;
                break;
            }
            else
            {
                /* Initialize revisions to minimum rev value (erased value) */
                for (BL_UINT8_T i = 0U; i < BL_NUM_PARTITIONS_TO_UPDATE; i++)
                {
                    partitions[i].revision = LOADER_PARTITION_INIT_REV;
                }
                state = GET_PARTITIONS;
            }
        }
        else
        {
            err = BL_ENODEV;
            break;
        }
    case GET_PARTITIONS:
        /* Read the partition information */
        if (NVM_Read(PARTITION_NODE, buf, &size) == BL_OK)
        {
            NVM_OperationFinish(PARTITION_NODE);
            state = CHECK_PARTITIONS;
        }
        else
        {
            break;
        }
    case CHECK_PARTITIONS:
        /* The always node to update is the app node, rev does not matter */
        partitions[0U].node = APPLICATION_NODE;
        for (BL_UINT8_T i = (PARTITION_NODE +
             APPLICATION_NODE);
             i < BL_NUM_PARTITIONS_TO_UPDATE;
             i++)
        {
            offset = i + PARTITION_NODE + APPLICATION_NODE;
            partitions[i].node = offset;
            UINT8_UINT32(&partitions[i].revision,
                         &buf[offset * LOADER_PARTITION_REV_SIZE]);
        }

        /* Sort the required nodes into info table */
        for (BL_UINT8_T j = (PARTITION_NODE +
             APPLICATION_NODE );
             j < BL_NUM_PARTITIONS_TO_UPDATE;
             j++)
        {
            for (BL_UINT8_T i = (PARTITION_NODE +
                 APPLICATION_NODE) * LOADER_PARTITION_REV_SIZE *
                 (j + 1U);
                 i < count * LOADER_PARTITION_REV_SIZE;
                 i += LOADER_PARTITION_REV_SIZE)
            {
                UINT8_UINT32(&info, &buf[i]);
                if (info > partitions[j].revision)
                {
                    partitions[j].revision = info;
                    partitions[j].node = i / LOADER_PARTITION_REV_SIZE;
                }
            }
        }

        /* Intentional fallthrough on state change */
        state = SIZE_PARTITIONS;
    case SIZE_PARTITIONS:
        /* Get partition sizes of required partitions */
        for (BL_UINT8_T i = 0U; i < BL_NUM_PARTITIONS_TO_UPDATE; i++)
        {
            if (NVM_GetSize(partitions[i].node, &partitions[i].size) != BL_OK)
            {
                err = BL_ENODEV;
                break;
            }
        }

        /* Ensure that there are available devices */
        if (err == BL_ENODEV)
        {
            break;
        }
        else
        {
            state = ERASE_PARTITIONS;
        }
    case ERASE_PARTITIONS:
        /* Erase the required partitions */
        if (NVM_Erase(partitions[erase].node, partitions[erase].size) == BL_OK)
        {
            NVM_OperationFinish(partitions[erase].node);
            erase++;
            if (erase >= BL_NUM_PARTITIONS_TO_UPDATE)
            {
                err = BL_OK;
                state = COUNT_PARTITIONS;
                erase = 0U;
            }
        }
        break;
    default:
        break;
    }

    return err;
}

/**@} loader */
