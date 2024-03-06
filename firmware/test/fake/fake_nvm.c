#include "fake_nvm.h"
#include <stddef.h>
#include <stdlib.h>

static uint8_t *buf = NULL;
static bool fail = false;

void Fake_NVMInit(void)
{
    if (!buf)
    {
        buf = calloc(FAKE_NVM_SIZE, sizeof(uint8_t));
        for (size_t i = 0; i < FAKE_NVM_SIZE; i++)
        {
            buf[i] = 0xFF;
        }
    }
}

void Fake_NVMDeinit(void)
{
}

bool Fake_NVMWrite(uint32_t address, uint8_t *data, uint32_t length)
{
    static bool ret = true;
    if (ret == true)
    {
        ret = false;
    }
    else if (address + length <= FAKE_NVM_SIZE)
    {
        for (size_t i = address; i < address + length; i++)
        {
            buf[i] = data[i];
        }
        ret = true;
    }
    return ret;
}

bool Fake_NVMRead(uint32_t address, uint8_t *data, uint32_t length)
{
    static bool ret = true;
    if (ret == true)
    {
        ret = false;
    }
    else if (address + length <= FAKE_NVM_SIZE)
    {
        for (size_t i = address; i < address + length; i++)
        {
            if (!fail)
            {
                data[i] = buf[i];
            }
            else
            {
                data[i] = 0xFF;
            }
        }
        ret = true;
        fail = false;
    }
    return ret;
}

bool Fake_NVMErase(uint32_t address, uint32_t length)
{
    static bool ret = true;
    if (ret == true)
    {
        ret = false;
    }
    else if (address + length <= FAKE_NVM_SIZE)
    {
        for (size_t i = address; i < address + length; i++)
        {
            buf[i] = 0xFF;
        }
        ret = true;
    }
    return ret;
}

void Fake_NVMFail(void)
{
    fail = true;
}
