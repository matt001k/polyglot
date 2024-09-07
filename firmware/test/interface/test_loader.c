#include "unity.h"
#include "config.h"
#include "loader.h"
#include "mock_table.h"
#include "mock_nvm.h"
#include "mock_ecc.h"
#include "mock_aes.h"
#include "mock_sha256.h"
#include "mock_buffer.h"
#include <stdbool.h>

#define RUN_DMA_OP(op) \
op##_ExpectAnyArgsAndReturn(BL_EINPROGRESS); \
TEST_ASSERT(Loader_Load() == BL_EINPROGRESS); \
op##_ExpectAnyArgsAndReturn(BL_OK);
#define LOADER_SIZE_BUF 512
#define PARTITION_SIZE ((LOADER_SIZE_BUF * 100) + (LOADER_SIZE_BUF / 3))
#define TEST_RUNS 5

static uint32_t partitionSize = PARTITION_SIZE;
static void loader_StartHelper(bool pass);
static void loader_TableHelper(void);
static void loader_EraseHelper(void);
static void loader_ReadHelper(void);
static void loader_DigestHelper(void);
static void loader_DecryptHelper(void);
static void loader_WriteHelper(void);
static void loader_FinishHelper(void);
static void loader_ValidateHelper(void);

void setUp(void)
{
}

void tearDown(void)
{
}

void test_LoaderFlow(void)
{
    uint32_t c = 0;
    for (uint8_t i = 0; i < TEST_RUNS; i++)
    {
        partitionSize = PARTITION_SIZE;
        c = 0;
        loader_StartHelper(true);
        TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
        loader_TableHelper();
        TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
        loader_EraseHelper();
        TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
        while (partitionSize)
        {
            loader_ReadHelper();
            TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
            loader_DigestHelper();
            TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
            loader_DecryptHelper();
            TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
            loader_WriteHelper();
            TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
            if (!partitionSize)
            {
                loader_FinishHelper();
                TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
            }
            else
            {
                TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
            }
            c++;
        }
        TEST_ASSERT(c == (PARTITION_SIZE % LOADER_SIZE_BUF == 0 ?
                    PARTITION_SIZE / LOADER_SIZE_BUF :
                    PARTITION_SIZE / LOADER_SIZE_BUF + 1));
        loader_ValidateHelper();
        TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
        TEST_ASSERT(Loader_Load() == BL_OK);
    }
}

static void loader_StartHelper(bool pass)
{
    Table_GetPartition_ExpectAndReturn(PARTITION_CURRENT, NULL, BL_OK);
    Table_GetPartition_IgnoreArg_node();
    NVM_GetSize_ExpectAndReturn(0, NULL, BL_OK);
    NVM_GetSize_IgnoreArg_node();
    NVM_GetSize_IgnoreArg_size();
    NVM_GetSize_ReturnThruPtr_size(&partitionSize);
    NVM_GetOperation_ExpectAnyArgsAndReturn(BL_OK);
    ECC_GetKey_ExpectAndReturn(BL_OK);
    AES_SetKey_ExpectAndReturn(BL_OK);
}

static void loader_TableHelper(void)
{
    static Table_Partition_t pt =
    {
        PARTITION_MAGIC,
        0, 
        0,
        0,
        0,
        {0},
        {0},
        {0},
    };
    static uint32_t size = sizeof(Table_Partition_t);
    pt.length = partitionSize;
    NVM_Read_ExpectAnyArgsAndReturn(BL_EINPROGRESS);
    TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
    NVM_Read_ExpectAndReturn(0, NULL, NULL, BL_OK);
    NVM_Read_IgnoreArg_node();
    NVM_Read_IgnoreArg_data();
    NVM_Read_IgnoreArg_length();
    NVM_Read_ReturnThruPtr_length(&size);
    NVM_Read_ReturnMemThruPtr_data((uint8_t *) &pt, size);
    NVM_OperationFinish_ExpectAnyArgsAndReturn(BL_OK);
    Table_WritePartition_ExpectAndReturn(PARTITION_CURRENT, NULL, BL_OK);
    Table_WritePartition_IgnoreArg_partition();
}

static void loader_EraseHelper(void)
{
    RUN_DMA_OP(NVM_Erase);
    NVM_OperationFinish_ExpectAnyArgsAndReturn(BL_OK);
}

static void loader_ReadHelper(void)
{
    static uint32_t size = 0;
    uint8_t p;
    size = partitionSize > LOADER_SIZE_BUF ? LOADER_SIZE_BUF : partitionSize;
    Buffer_Get_ExpectAndReturn(&p);
    NVM_Read_ExpectAnyArgsAndReturn(BL_EINPROGRESS);
    TEST_ASSERT(Loader_Load() == BL_EINPROGRESS);
    Buffer_Get_ExpectAndReturn(&p);
    NVM_Read_ExpectAndReturn(0, NULL, NULL, BL_OK);
    NVM_Read_IgnoreArg_node();
    NVM_Read_IgnoreArg_data();
    NVM_Read_IgnoreArg_length();
    NVM_Read_ReturnThruPtr_length(&size);
    partitionSize -= size;
}

static void loader_DigestHelper(void)
{
    RUN_DMA_OP(SHA256_Calculate);
}

static void loader_DecryptHelper(void)
{
    RUN_DMA_OP(AES_Decrypt);
}

static void loader_WriteHelper(void)
{
    RUN_DMA_OP(NVM_Write);
}

static void loader_FinishHelper(void)
{
    RUN_DMA_OP(SHA256_Finish);
    NVM_OperationFinish_ExpectAnyArgsAndReturn(BL_OK);
    NVM_OperationFinish_ExpectAndReturn(APPLICATION_NODE, BL_OK);
}

static void loader_ValidateHelper(void)
{
    RUN_DMA_OP(ECC_Decrypt);
    Table_UpdatePartitions_ExpectAndReturn(BL_OK);
}
