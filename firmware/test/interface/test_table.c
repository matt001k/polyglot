#include "fake_nvm.h"
#include "nvm.h"
#include "table.h"
#include "unity.h"

#include <string.h>
TEST_FILE("nvm.c")
TEST_FILE("helper.c")
TEST_FILE("fake_nvm.c")

#define WRITE_READ_COMPARE(table, pass)                                        \
  TEST_ASSERT(Table_WritePartition(PARTITION_CURRENT,                          \
                                   &partition[WRITE_PARTITION]) == BL_OK);     \
  TEST_ASSERT(Table_ReadPartition(PARTITION_CURRENT,                           \
                                  &partition[READ_PARTITION]) == BL_OK);       \
  TEST_ASSERT(memcmp(&partition[WRITE_PARTITION],                              \
                     &partition[READ_PARTITION],                               \
                     sizeof(Table_Partition_t)) pass 0)

#define FAKE_PARTITION                                                         \
  .magic = PARTITION_MAGIC, .crc = 0xFCFCFCFC, .length = 0xFFFFF,              \
  .signature = { 0 }, .reserved1 = 0, .reserved2 = 0, .reserved3 = { 0 },

enum {
  WRITE_PARTITION,
  READ_PARTITION,
  COMPARE_COUNT,
};

void setUp(void)
{
  Fake_NVMInit();
  NVM_Init();
  Table_Init();
}

void tearDown(void)
{
  while(NVM_Erase(0, FAKE_NVM_SIZE) != BL_OK)
    ;
  NVM_OperationFinish(0);
  Fake_NVMDeinit();
  Table_Deinit();
}

void test_TableInit(void)
{
  Table_Partition_t partition = { FAKE_PARTITION };
  tearDown();
  while(NVM_Write(PARTITION_NODE, (uint8_t *)&partition, sizeof(partition)) !=
        BL_OK)
    ;
  NVM_OperationFinish(PARTITION_NODE);
  TEST_ASSERT(Table_Init() == BL_OK);
}

void test_TableWritePartition(void)
{
  Table_Partition_t partition = { FAKE_PARTITION };
  TEST_ASSERT(Table_WritePartition(PARTITION_CURRENT, &partition) == BL_OK);
  TEST_ASSERT(Table_WritePartition(PARTITION_PREVIOUS, &partition) == BL_OK);
  TEST_ASSERT(Table_WritePartition(PARTITION_NEXT, &partition) == BL_OK);

  /* Test invalid conditions */
  TEST_ASSERT(Table_WritePartition(PARTITION_COUNT, &partition) != BL_OK);
  TEST_ASSERT(Table_WritePartition(PARTITION_NEXT, NULL) != BL_OK);
}

void test_TableReadPartition(void)
{
  Table_Partition_t partition = { 0 };
  TEST_ASSERT(Table_ReadPartition(PARTITION_CURRENT, &partition) == BL_OK);
  TEST_ASSERT(Table_ReadPartition(PARTITION_PREVIOUS, &partition) == BL_OK);
  TEST_ASSERT(Table_ReadPartition(PARTITION_NEXT, &partition) == BL_OK);

  /* Test invalid conditions */
  TEST_ASSERT(Table_ReadPartition(PARTITION_COUNT, &partition) != BL_OK);
  TEST_ASSERT(Table_ReadPartition(PARTITION_NEXT, NULL) != BL_OK);
}

void test_TableGetPartition(void)
{
  NVM_Node_t node = 0;
  /* Test getting the 3 potential partitions */
  TEST_ASSERT(Table_GetPartition(PARTITION_CURRENT, &node) == BL_OK);
  TEST_ASSERT(node == NUM_DEFAULT_NODES);
  TEST_ASSERT(Table_GetPartition(PARTITION_NEXT, &node) == BL_OK);
  TEST_ASSERT(node == NUM_DEFAULT_NODES + 1);
  TEST_ASSERT(Table_GetPartition(PARTITION_PREVIOUS, &node) == BL_OK);
  TEST_ASSERT(node == NUM_NVM_NODES - 1);

  /* Test invalid use cases */
  TEST_ASSERT(Table_GetPartition(PARTITION_CURRENT, NULL) != BL_OK);
  TEST_ASSERT(Table_GetPartition(PARTITION_COUNT, &node) != BL_OK);
  Table_Deinit();
  TEST_ASSERT(Table_GetPartition(PARTITION_CURRENT, &node) != BL_OK);
}

void test_TableWriteReadPartition(void)
{
  Table_Partition_t partition[COMPARE_COUNT] = {
    { FAKE_PARTITION },
    { 0 },
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
  Table_Partition_t partition[COMPARE_COUNT] = {
    { FAKE_PARTITION },
    { 0 },
  };
  TEST_ASSERT(Table_WritePartition(PARTITION_CURRENT,
                                   &partition[WRITE_PARTITION]) == BL_OK);
  TEST_ASSERT(Table_UpdatePartitions() == BL_OK);
  TEST_ASSERT(Table_ReadPartition(PARTITION_CURRENT,
                                  &partition[READ_PARTITION]) == BL_OK);
  TEST_ASSERT(memcmp(&partition[WRITE_PARTITION],
                     &partition[READ_PARTITION],
                     sizeof(Table_Partition_t)) != 0);
  TEST_ASSERT(Table_ReadPartition(PARTITION_PREVIOUS,
                                  &partition[READ_PARTITION]) == BL_OK);
  TEST_ASSERT(memcmp(&partition[WRITE_PARTITION],
                     &partition[READ_PARTITION],
                     sizeof(Table_Partition_t)) == 0);
  /* Get full branch coverage */
  for(uint8_t i = 0; i < NUM_NVM_NODES; i++) {
    TEST_ASSERT(Table_UpdatePartitions() == BL_OK);
  }
}

void test_TableRevertPartitions(void)
{
  Table_Partition_t partition[COMPARE_COUNT] = {
    { FAKE_PARTITION },
    { 0 },
  };
  TEST_ASSERT(Table_UpdatePartitions() == BL_OK);
  TEST_ASSERT(Table_WritePartition(PARTITION_CURRENT,
                                   &partition[WRITE_PARTITION]) == BL_OK);
  TEST_ASSERT(Table_RevertPartitions() == BL_OK);

  /* Current partition should not be same as reverted */
  TEST_ASSERT(Table_ReadPartition(PARTITION_CURRENT,
                                  &partition[READ_PARTITION]) == BL_OK);
  TEST_ASSERT(memcmp(&partition[WRITE_PARTITION],
                     &partition[READ_PARTITION],
                     sizeof(Table_Partition_t)) != 0);
  /* Next partition will be same as reverted */
  TEST_ASSERT(Table_ReadPartition(PARTITION_NEXT, &partition[READ_PARTITION]) ==
              BL_OK);
  TEST_ASSERT(memcmp(&partition[WRITE_PARTITION],
                     &partition[READ_PARTITION],
                     sizeof(Table_Partition_t)) == 0);
  /* Get full branch coverage */
  for(uint8_t i = 0; i < NUM_NVM_NODES; i++) {
    TEST_ASSERT(Table_RevertPartitions() == BL_OK);
  }
}

void test_TableCheckValid(void)
{

  Table_Partition_t partition = { 0 };
  /* All partitions should be valid if initially loaded */
  TEST_ASSERT(Table_CheckValid(PARTITION_CURRENT) == BL_OK);
  TEST_ASSERT(Table_CheckValid(PARTITION_PREVIOUS) == BL_OK);
  TEST_ASSERT(Table_CheckValid(PARTITION_NEXT) == BL_OK);

  /* Ensure that a table will be invalidated if bad data written */
  TEST_ASSERT(Table_WritePartition(PARTITION_CURRENT, &partition) == BL_OK);
  TEST_ASSERT(Table_CheckValid(PARTITION_CURRENT) != BL_OK);

  /* Test invalid conditions */
  TEST_ASSERT(Table_CheckValid(PARTITION_COUNT) != BL_OK);
}
