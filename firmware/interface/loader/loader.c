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
#include "table.h"
#include "buffer.h"
#include "helper.h"
#include "aes.h"
#include "sha256.h"
#include "verify.h"

#define LOADER_CHECK(s, f) s = loader_Check(s, f);

typedef enum
{
    LOADER_INIT,
    LOADER_TABLE,
    LOADER_ERASE,
    LOADER_READ,
    LOADER_DIGEST,
    LOADER_DECRYPT,
    LOADER_WRITE,
    LOADER_CHECK,
    LOADER_VALIDATE,
    LOADER_CLEAN,
} states_e;

BL_STATIC struct loader_s
{
    struct
    {
        BL_UINT32_T buf;
        BL_UINT32_T total;
        BL_UINT32_T partition;
    } size;
    Table_Partition_t table;
    struct
    {
        BL_UINT8_T *in;
        BL_UINT8_T *out;
    } p;
    BL_UINT8_T digest[SHA256_DIGEST_SIZE];
    NVM_Node_t node;
    BL_UINT8_T clean;
} loader = {0};

BL_STATIC BL_CONST BL_Err_t acceptable[] =
{
    BL_EINPROGRESS,
};

BL_STATIC_INLINE states_e loader_Check(states_e current, BL_Err_t err);
BL_STATIC BL_Err_t loader_Start(void);
BL_STATIC BL_Err_t loader_Table(void);
BL_STATIC BL_Err_t loader_Erase(void);
BL_STATIC BL_Err_t loader_Read(void);
BL_STATIC BL_Err_t loader_Digest(void);
BL_STATIC BL_Err_t loader_Decrypt(void);
BL_STATIC BL_Err_t loader_Write(void);
BL_STATIC states_e loader_Finish(BL_Err_t *err);
BL_STATIC BL_Err_t loader_Validate(void);

BL_Err_t Loader_Load(void)
{
    static states_e state = LOADER_INIT;
    BL_Err_t err = BL_EINPROGRESS;
    
    switch (state)
    {
        case LOADER_INIT:
            LOADER_CHECK(state, loader_Start());
            break;
        case LOADER_TABLE:
            LOADER_CHECK(state, loader_Table());
            break;
        case LOADER_ERASE:
            LOADER_CHECK(state, loader_Erase());
            break;
        case LOADER_READ:
            LOADER_CHECK(state, loader_Read());
            break;
        case LOADER_DIGEST:
            LOADER_CHECK(state, loader_Digest());
            break;
        case LOADER_DECRYPT:
            LOADER_CHECK(state, loader_Decrypt());
            break;
        case LOADER_WRITE:
            LOADER_CHECK(state, loader_Write());
            break;
        case LOADER_CHECK:
            state = loader_Finish(&err);
            break;
        case LOADER_VALIDATE:
            LOADER_CHECK(state, loader_Validate());
            break;
        case LOADER_CLEAN:
            if (loader.clean)
            {
                err = BL_OK;
            }
            else
            {
                err = BL_ERR;
            }
            state = LOADER_INIT;
            loader = (struct loader_s) {0};
            break;
    }
    return err;
}

BL_STATIC_INLINE states_e loader_Check(states_e current, BL_Err_t err)
{
    states_e state = LOADER_CLEAN;
    if (err == BL_EINPROGRESS)
    {
        state = current;
    }
    else if (err == BL_OK)
    {
        state = ++current;
    }
    return state;
}

BL_STATIC BL_Err_t loader_Start(void)
{
    BL_Err_t err = BL_ERR;
    NVM_Operation_t op = NVM_NONE_OP;
    if((err = Table_GetPartition(PARTITION_CURRENT, &loader.node)) == BL_OK &&
        (err = NVM_GetSize(loader.node, &loader.size.partition)) == BL_OK &&
        (err = NVM_GetOperation(loader.node, &op)) == BL_OK &&
        (err = Verify_GetKey()) == BL_OK &&
        (err = AES_SetKey()) == BL_OK)
    {
        if (op != NVM_NONE_OP)
        {
            err = BL_ERR;
        }
    }
    return err;
}

BL_STATIC BL_Err_t loader_Table(void)
{
    BL_Err_t err = BL_ERR;
    BL_UINT32_T size = 0;
    size = sizeof(loader.table);
    err = NVM_Read(loader.node,
                   (BL_UINT8_T *) &loader.table,
                   &size);
    if (err == BL_OK && size &&
            (err = NVM_OperationFinish(loader.node)) == BL_OK)
    {

        err = Table_WritePartition(PARTITION_CURRENT,
                                  &loader.table);
    }
    return err;
}

BL_STATIC BL_Err_t loader_Erase(void)
{
    BL_Err_t err = BL_ERR;
    err = NVM_Erase(loader.node,
                   loader.size.partition);
    if (err == BL_OK)
    {
        err = NVM_OperationFinish(loader.node);
    }
    return err;
}

BL_STATIC BL_Err_t loader_Read(void)
{
    BL_UINT8_T *data = Buffer_Get();
    BL_Err_t err = BL_ERR;
    loader.size.buf = (BL_BUFFER_SIZE / 2) - (BL_BUFFER_SIZE / 2) % AES_IV_SIZE;
    loader.p.in = data;
    loader.p.out = data + loader.size.buf;
    err = NVM_Read(loader.node,
                   data,
                   &loader.size.buf);
    return err;
}

BL_STATIC BL_Err_t loader_Digest(void)
{
    BL_Err_t err = BL_ERR;
    err = SHA256_Calculate(loader.p.in, loader.size.buf);
    return err;
}

BL_STATIC BL_Err_t loader_Decrypt(void)
{
    BL_Err_t err = BL_ERR;
    err = AES_Decrypt(loader.p.in,
                      loader.p.out,
                      loader.size.buf);
    return err;
}

BL_STATIC BL_Err_t loader_Write(void)
{
    BL_Err_t err = BL_ERR;
    err = NVM_Write(APPLICATION_NODE, loader.p.out, loader.size.buf);
    loader.size.total += loader.size.buf;
    return err;
}

BL_STATIC states_e loader_Finish(BL_Err_t *err)
{
    states_e state = LOADER_CHECK;
    if (loader.size.total >= loader.table.length)
    {
        if ((*err = SHA256_Finish(loader.digest)) == BL_OK &&
             (*err = NVM_OperationFinish(loader.node)) == BL_OK &&
             (*err = NVM_OperationFinish(APPLICATION_NODE)) == BL_OK)
        {
            state = LOADER_VALIDATE;
            *err = BL_EINPROGRESS;
        }
        else if (*err == BL_ERR)
        {
            state = LOADER_CLEAN;
        }
    }
    else
    {
        state = LOADER_READ;
        *err = BL_EINPROGRESS;
    }
    return state;
}

BL_STATIC BL_Err_t loader_Validate(void)
{
    BL_Err_t err = BL_ERR;
    if((err = Verify_Decrypt(loader.digest, loader.table.signature)) == BL_OK &&
            (err = Table_UpdatePartitions()) == BL_OK)
    {
        loader.clean = FLAG_SET;
    }
    return err;
}
