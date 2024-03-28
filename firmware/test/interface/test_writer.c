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

#define TEST_PARTITION 2
#define START_HELPER(partition, ret) \
writer.node = partition; \
Table_GetPartition_ExpectAndReturn(PARTITION_NEXT, NULL, ret); \
Table_GetPartition_IgnoreArg_node(); \
Table_GetPartition_ReturnThruPtr_node(&writer.node);

#define RUN_WRITER_OP(op) \
TEST_ASSERT(op == BL_EINPROGRESS); \
TEST_ASSERT(op == BL_OK);

#define START_WRITER_OP() RUN_WRITER_OP(Writer_Start())
#define WRITE_WRITER_OP(data, size) RUN_WRITER_OP(Writer_WriteData(data, size))
#define READ_WRITER_OP(node, data, size) RUN_WRITER_OP(NVM_Read(node, data, size))

static bool valid_return(BL_Err_t err);

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
    RandomFree(writer.write.buf);
}

void test_WriterStart(void)
{
    bool valid = true;
    START_HELPER(TEST_PARTITION, BL_OK);
    START_WRITER_OP();

    /* Test already started condition */
    TEST_ASSERT(Writer_Start() == BL_EALREADY);
    TEST_ASSERT(Writer_Finish() == BL_OK);

    /* Test invalid conditions */
    START_HELPER(TEST_PARTITION, BL_OK);
    NVM_Deinit();
    TEST_ASSERT_EQUAL(false, valid_return(Writer_Start()));
    NVM_Init();

    /* Test another operation ongoing when started */
    START_HELPER(TEST_PARTITION, BL_OK);
    NVM_Read(TEST_PARTITION, writer.read.buf, &writer.read.size);
    TEST_ASSERT_EQUAL(false, valid_return(Writer_Start()));
    NVM_OperationFinish(TEST_PARTITION);

    /* Improper table operation */
    START_HELPER(TEST_PARTITION, BL_ERR);
    TEST_ASSERT_EQUAL(false, valid_return(Writer_Start()));

    /* Improper node obtained */
    START_HELPER(UINT8_MAX, BL_OK);
    TEST_ASSERT_EQUAL(false, valid_return(Writer_Start()));
}

void test_WriterWrite(void)
{
    /* Test invalid conditions */
    TEST_ASSERT(Writer_WriteData(NULL, FAKE_NVM_SIZE) != BL_OK);
    TEST_ASSERT(Writer_WriteData(writer.write.buf, 0) != BL_OK);
    TEST_ASSERT(Writer_WriteData(writer.write.buf, FAKE_NVM_SIZE) != BL_OK);

    /* Start a read condition before a write */
    START_HELPER(TEST_PARTITION, BL_OK);
    START_WRITER_OP();
    READ_WRITER_OP(TEST_PARTITION, writer.read.buf, &writer.read.size);
    TEST_ASSERT(Writer_WriteData(writer.write.buf,
                                 writer.write.size) != BL_OK);
    TEST_ASSERT(Writer_Finish() == BL_OK);
}

void test_WriterWriteEraseRead(void)
{
    START_HELPER(TEST_PARTITION, BL_OK);
    START_WRITER_OP();
    WRITE_WRITER_OP(writer.write.buf, FAKE_NVM_SIZE);
    TEST_ASSERT(Writer_Finish() == BL_OK);
    READ_WRITER_OP(TEST_PARTITION, writer.read.buf, &writer.read.size);
    TEST_ASSERT(memcmp(writer.read.buf,
                       writer.write.buf,
                       writer.read.size) == 0);
    TEST_ASSERT(writer.read.size == FAKE_NVM_SIZE);
    NVM_OperationFinish(2);
}

void test_WriterFinish(void)
{
    /* Test invalid use cases */
    TEST_ASSERT(Writer_Finish() == BL_EACCES);
}

void test_WriterStates(void)
{
    struct
    {
        BL_Err_t *err;
        BL_UINT8_T count;
    } acceptable = {0};

    /* Test invalid use cases */
    Writer_States(&acceptable.err, NULL);
    Writer_States(NULL, &acceptable.count);
    Writer_States(NULL, NULL);
}

static bool valid_return(BL_Err_t err)
{
    bool ret = err == BL_OK ? true : false;
    struct
    {
        BL_Err_t *err;
        BL_UINT8_T count;
    } acceptable = {0};
    Writer_States(&acceptable.err, &acceptable.count);

    if (ret == false && acceptable.err)
    {
        for (uint8_t i = 0; i < acceptable.count; i++)
        {
            if (err == acceptable.err[i])
            {
                ret = true;
                break;
            }
        }
    }
    return ret;
}
