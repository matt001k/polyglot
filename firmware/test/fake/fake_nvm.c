#include "fake_nvm.h"
#include <stddef.h>
#include <stdlib.h>

static uint8_t *buf = NULL;
static bool fail = false;
static struct ret_s
{
    bool write;
    bool read;
    bool erase;
} ret = {true, true, true};

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
    ret = (struct ret_s) {true, true, true};
}

bool Fake_NVMWrite(uint32_t address, uint8_t *data, uint32_t length)
{
    if (ret.write == true)
    {
        ret.write = false;
    }
    else if (address + length <= FAKE_NVM_SIZE)
    {
        for (size_t i = address; i < address + length; i++)
        {
            buf[i] = data[i];
        }
        ret.write = true;
    }
    return ret.write;
}

bool Fake_NVMRead(uint32_t address, uint8_t *data, uint32_t length)
{
    if (ret.read == true)
    {
        ret.read = false;
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
        ret.read = true;
        fail = false;
    }
    return ret.read;
}

bool Fake_NVMErase(uint32_t address, uint32_t length)
{
    if (ret.erase == true)
    {
        ret.erase = false;
    }
    else if (address + length <= FAKE_NVM_SIZE)
    {
        for (size_t i = address; i < address + length; i++)
        {
            buf[i] = 0xFF;
        }
        ret.erase = true;
    }
    return ret.erase;
}

void Fake_NVMFail(void)
{
    fail = true;
}
