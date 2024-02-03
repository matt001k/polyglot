#include "nvm.h"
#include "unity.h"
#include "jump.h"
#include "mock_nvm.h"
#include <stdlib.h>

#define JUMP_ADDRESS 0x1000
static void jumpHere(uint32_t address);

Jump_Cfg_t *jCfg;

void setUp(void)
{
}

void tearDown(void)
{
    Jump_DeInit();
}

void test_JumpFail(void)
{
    jCfg = calloc(1, sizeof(Jump_Cfg_t));
    jCfg[0] = (Jump_Cfg_t) { .cb = NULL };
    /* There is no cb function */
    TEST_ASSERT(Jump_Init() != BL_OK);
    TEST_ASSERT(Jump_ToApp() != BL_OK);
    free(jCfg);
}

void test_JumpToApp(void)
{
    static uint32_t address = JUMP_ADDRESS;
    jCfg = calloc(2, sizeof(Jump_Cfg_t));
    jCfg[0] = (Jump_Cfg_t) { .cb = jumpHere};
    jCfg[1] = (Jump_Cfg_t) { .cb = NULL };
    TEST_ASSERT(Jump_Init() == BL_OK);

    /* Proper retrieval of jump location */
    NVM_GetLocation_ExpectAnyArgsAndReturn(BL_OK);
    NVM_GetLocation_ReturnMemThruPtr_location(&address, sizeof(uint32_t));
    TEST_ASSERT(Jump_ToApp() == BL_OK);

    /* Failure of retrieval of jump location */
    NVM_GetLocation_ExpectAnyArgsAndReturn(BL_ERR);
    TEST_ASSERT(Jump_ToApp() != BL_OK);
    free(jCfg);
}

static void jumpHere(uint32_t address)
{
    TEST_ASSERT(address == JUMP_ADDRESS);
}
