#include "unity.h"
#include "table.h"
#include "nvm.h"
#include "fake_nvm.h"
#include <string.h>
TEST_FILE("nvm.c")
TEST_FILE("helper.c")
TEST_FILE("fake_nvm.c")

#define WRITE_READ_COMPARE(table, pass) \
    TEST_ASSERT(Table_WritePartiton(PARTITION_CURRENT, \
                                    &partition[WRITE_PARTITION]) == BL_OK); \
    TEST_ASSERT(Table_ReadPartiton(PARTITION_CURRENT, \
                                   &partition[READ_PARTITION]) == BL_OK); \
    TEST_ASSERT(memcmp(&partition[WRITE_PARTITION], \
                       &partition[READ_PARTITION], \
                       sizeof(Table_Partition_t)) pass 0)

#define FAKE_PARTITION \
    .magic = PARTITION_MAGIC, \
    .crc = 0xFCFCFCFC, \
    .length = 0xFFFFF, \
    .signature = {0}, \
    .reserved1 = 0, \
    .reserved2 = 0, \
    .reserved3 = {0},

enum
{
    WRITE_PARTITION,
    READ_PARTITION,
    COMPARE_COUNT,
};


void setUp(void)
{
    NVM_Init();
    Table_Init();
}

void tearDown(void)
{
    while(NVM_Erase(0, FAKE_NVM_SIZE) != BL_OK);
    NVM_OperationFinish(0);
    Fake_NVMDeinit();
}

void test_TableWritePartition(void)
{
    Table_Partition_t partition =
    {
        FAKE_PARTITION
    };
    TEST_ASSERT(Table_WritePartiton(PARTITION_CURRENT, &partition) == BL_OK);
    TEST_ASSERT(Table_WritePartiton(PARTITION_PREVIOUS, &partition) == BL_OK);
    TEST_ASSERT(Table_WritePartiton(PARTITION_NEXT, &partition) == BL_OK);

    /* Test invalid conditions */
    TEST_ASSERT(Table_WritePartiton(PARTITION_COUNT, &partition) != BL_OK);
    TEST_ASSERT(Table_WritePartiton(PARTITION_NEXT, NULL) != BL_OK);
}

void test_TableReadPartition(void)
{
    Table_Partition_t partition = {0};
    TEST_ASSERT(Table_ReadPartiton(PARTITION_CURRENT, &partition) == BL_OK);
    TEST_ASSERT(Table_ReadPartiton(PARTITION_PREVIOUS, &partition) == BL_OK);
    TEST_ASSERT(Table_ReadPartiton(PARTITION_NEXT, &partition) == BL_OK);

    /* Test invalid conditions */
    TEST_ASSERT(Table_ReadPartiton(PARTITION_COUNT, &partition) != BL_OK);
    TEST_ASSERT(Table_ReadPartiton(PARTITION_NEXT, NULL) != BL_OK);
}

void test_TableWriteReadPartition(void)
{
    Table_Partition_t partition[COMPARE_COUNT] =
    {
        {
            FAKE_PARTITION
        },
        {0},
    };
    WRITE_READ_COMPARE(PARTITION_CURRENT, ==);
    WRITE_READ_COMPARE(PARTITION_PREVIOUS, ==);
    WRITE_READ_COMPARE(PARTITION_NEXT, ==);
    /* Test failure */
    Fake_NVMFail();
    WRITE_READ_COMPARE(PARTITION_NEXT, !=);
}

void test_TableUpdatePartitions(void)
{
    Table_Partition_t partition[COMPARE_COUNT] =
    {
        {
            FAKE_PARTITION
        },
        {0},
    };
    TEST_ASSERT(Table_WritePartiton(PARTITION_CURRENT,
                                    &partition[WRITE_PARTITION]) == BL_OK);
    TEST_ASSERT(Table_UpdatePartitions() == BL_OK);
    TEST_ASSERT(Table_ReadPartiton(PARTITION_CURRENT,
                                    &partition[READ_PARTITION]) == BL_OK);
    TEST_ASSERT(memcmp(&partition[WRITE_PARTITION], \
                       &partition[READ_PARTITION], \
                       sizeof(Table_Partition_t)) != 0);
}

void test_TableRevertPartitions(void)
{
    Table_Partition_t partition[COMPARE_COUNT] =
    {
        {
            FAKE_PARTITION
        },
        {0},
    };
    /* Cannot revert if never been updated */
    TEST_ASSERT(Table_RevertPartitions() != BL_OK);

    TEST_ASSERT(Table_UpdatePartitions() == BL_OK);
    TEST_ASSERT(Table_WritePartiton(PARTITION_CURRENT,
                                    &partition[WRITE_PARTITION]) == BL_OK);
    TEST_ASSERT(Table_RevertPartitions() == BL_OK);

    /* Cannot revert twice in a row */
    TEST_ASSERT(Table_RevertPartitions() != BL_OK);

    /* Current partition should not be same as reverted */
    TEST_ASSERT(Table_ReadPartiton(PARTITION_CURRENT,
                                    &partition[READ_PARTITION]) == BL_OK);
    TEST_ASSERT(memcmp(&partition[WRITE_PARTITION], \
                       &partition[READ_PARTITION], \
                       sizeof(Table_Partition_t)) != 0);
    /* Next partition will be same as reverted */
    TEST_ASSERT(Table_ReadPartiton(PARTITION_NEXT,
                                    &partition[READ_PARTITION]) == BL_OK);
    TEST_ASSERT(memcmp(&partition[WRITE_PARTITION], \
                       &partition[READ_PARTITION], \
                       sizeof(Table_Partition_t)) == 0);
}

void test_TableCheckValdi(void)
{

    Table_Partition_t partition = {0};
    /* All partitions should be valid if initially loaded */
    TEST_ASSERT(Table_CheckValid(PARTITION_CURRENT) == BL_OK);
    TEST_ASSERT(Table_CheckValid(PARTITION_PREVIOUS) == BL_OK);
    TEST_ASSERT(Table_CheckValid(PARTITION_NEXT) == BL_OK);

    /* Ensure that a table will be invalidated if bad data written */
    TEST_ASSERT(Table_WritePartiton(PARTITION_CURRENT,
                                    &partition) == BL_OK);
    TEST_ASSERT(Table_CheckValid(PARTITION_CURRENT) != BL_OK);

    /* Test invalid conditions */
    TEST_ASSERT(Table_CheckValid(PARTITION_COUNT) != BL_OK);
}
