#include <stdint.h>
#include <stdbool.h>

#define FAKE_NVM_SIZE 0xFFFFF
#define FAKE_NVM_LOCATION 0x0
#define FAKE_NVM_SECTOR_SIZE 0x1000

void Fake_NVMInit(void);
void Fake_NVMDeinit(void);
bool Fake_NVMWrite(uint32_t address, uint8_t *data, uint32_t length);
bool Fake_NVMRead(uint32_t address, uint8_t *data, uint32_t length);
bool Fake_NVMErase(uint32_t address, uint32_t length);
void Fake_NVMFail(void);
