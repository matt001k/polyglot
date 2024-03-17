#include "unity.h"
#include "config.h"
#include "writer.h"
#include "mock_table.h"
#include "fake_nvm.h"
#include "randomizer.h"
#include <stdlib.h>
#include <string.h>
TEST_FILE("nvm.c")
TEST_FILE("helper.c")
TEST_FILE("fake_nvm.c")
TEST_FILE("randomizer.c")

#define START_HELPER(partition) \
writer.node = partition; \
Table_GetPartition_ExpectAndReturn(PARTITION_CURRENT, NULL, BL_OK); \
Table_GetPartition_IgnoreArg_node(); \
Table_GetPartition_ReturnThruPtr_node(&writer.node);

#define RUN_WRITER_OP(op) \
TEST_ASSERT(op != BL_OK); \
TEST_ASSERT(op == BL_OK);

#define ERASE_WRITER_OP() RUN_WRITER_OP(Writer_Erase())
#define WRITE_WRITER_OP(data, size) RUN_WRITER_OP(Writer_WriteData(data, size))
#define READ_WRITER_OP(data, size) RUN_WRITER_OP(Writer_ReadData(data, size))

static struct
{
    NVM_Node_t node;
    struct write_s
    {
        uint8_t *buf;
        uint32_t size;
    } write;
    struct read_s
    {
        uint8_t *buf;
        uint32_t size;
    } read;
} writer = {0};

void setUp(void)
{
    NVM_Init();
    writer.read = (struct read_s)
    {
        (uint8_t *) malloc(FAKE_NVM_SIZE),
        FAKE_NVM_SIZE,
    };
    writer.write.buf = RandomArray(FAKE_NVM_SIZE);
    writer.write.size = FAKE_NVM_SIZE;
}

void tearDown(void)
{
    while(NVM_Erase(0, FAKE_NVM_SIZE) != BL_OK);
    NVM_OperationFinish(0);
    while(NVM_Erase(1, FAKE_NVM_SIZE) != BL_OK);
    NVM_OperationFinish(1);
    while(NVM_Erase(2, FAKE_NVM_SIZE) != BL_OK);
    NVM_OperationFinish(2);
    while(NVM_Erase(3, FAKE_NVM_SIZE) != BL_OK);
    NVM_OperationFinish(3);
    Fake_NVMDeinit();
    free(writer.read.buf);
}

void test_WriterStart(void)
{
    START_HELPER(2);
    TEST_ASSERT(Writer_Start() == BL_OK);

    /* Test invalid conditions */
    Table_GetPartition_ExpectAndReturn(PARTITION_CURRENT, NULL, BL_ERR);
    Table_GetPartition_IgnoreArg_node();
    TEST_ASSERT(Writer_Start() != BL_OK);
    TEST_ASSERT(Writer_Finish() == BL_OK);

    START_HELPER(2);
    NVM_Deinit();
    TEST_ASSERT(Writer_Start() != BL_OK);
    NVM_Init();
}

void test_WriterErase(void)
{
    /* Test invalid conditions */
    TEST_ASSERT(Writer_Erase() != BL_OK);

    START_HELPER(2);
    TEST_ASSERT(Writer_Start() == BL_OK);
    ERASE_WRITER_OP();
    TEST_ASSERT(Writer_Finish() == BL_OK);
}

void test_WriterWrite(void)
{
    /* Test invalid conditions */
    TEST_ASSERT(Writer_WriteData(NULL, FAKE_NVM_SIZE) != BL_OK);
    TEST_ASSERT(Writer_WriteData(writer.write.buf, 0) != BL_OK);
    TEST_ASSERT(Writer_WriteData(writer.write.buf, FAKE_NVM_SIZE) != BL_OK);

    /* Start a read condition before a write */
    START_HELPER(2);
    TEST_ASSERT(Writer_Start() == BL_OK);
    READ_WRITER_OP(writer.read.buf, &writer.read.size);
    TEST_ASSERT(Writer_WriteData(writer.write.buf,
                                 writer.write.size) != BL_OK);
    TEST_ASSERT(Writer_Finish() == BL_OK);
}

void test_WriterRead(void)
{
    /* Test invalid conditions */
    TEST_ASSERT(Writer_ReadData(NULL, &writer.read.size) != BL_OK);
    TEST_ASSERT(Writer_ReadData(writer.read.buf, 0) != BL_OK);
    TEST_ASSERT(Writer_ReadData(writer.read.buf, &writer.read.size) != BL_OK);

    /* Start a write condition before a read */
    START_HELPER(2);
    TEST_ASSERT(Writer_Start() == BL_OK);
    WRITE_WRITER_OP(writer.write.buf, writer.write.size);
    TEST_ASSERT(Writer_ReadData(writer.read.buf,
                                &writer.read.size) != BL_OK);
    TEST_ASSERT(Writer_Finish() == BL_OK);
}

void test_WriterWriteEraseRead(void)
{
    START_HELPER(2);
    TEST_ASSERT(Writer_Start() == BL_OK);
    ERASE_WRITER_OP();
    WRITE_WRITER_OP(writer.write.buf, FAKE_NVM_SIZE);
    TEST_ASSERT(Writer_Finish() == BL_OK);
    START_HELPER(2);
    TEST_ASSERT(Writer_Start() == BL_OK);
    READ_WRITER_OP(writer.read.buf, &writer.read.size);
    TEST_ASSERT(Writer_Finish() == BL_OK);
    TEST_ASSERT(memcmp(writer.read.buf,
                       writer.write.buf,
                       writer.read.size) == 0);
    TEST_ASSERT(writer.read.size == FAKE_NVM_SIZE);
}
