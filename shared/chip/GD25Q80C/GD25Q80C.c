/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup GD25Q80C
 * @{
 */

/**
 * @file        GD25Q80C.c
 *
 * @brief       Driver for GD25Q80C IC
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#include "GD25Q80C.h"

#define GD25Q80C_NUM_STATUS_REGISTERS (1U)

#define GD25Q80C_STATUS_WIP_BIT (0b00000001)
#define GD25Q80C_STATUS_WEL_BIT (0b00000010)

#define GD25Q80C_TRANSFER_INPROGRESS (1U)
#define GD25Q80C_TRANSFER_COMPLETE (0U)

#define RESET_DELAY_US (2500)
#define GET_BITS_23_16(val) ((uint8_t) ((val & 0x00FF0000) >> 16U))
#define GET_BITS_15_8(val) ((uint8_t) ((val & 0x0000FF00) >> 8U))
#define GET_BITS_7_0(val) ((uint8_t) (val & 0x000000FF))
#define ASSIGN_DATA_STRUCT(s, c, cS, tD, rD, dS)    \
do                                                  \
{                                                   \
    s.cmd = c;                                      \
    s.cSize = cS;                                   \
    s.tx = tD;                                      \
    s.rx = rD;                                      \
    s.dSize = dS;                                   \
} while (0U)
#define ATOMIC_ACCESS(inst, fn) \
do                              \
{                               \
    if(inst->cfg.enter)         \
    {                           \
        inst->cfg.enter();      \
    }                           \
    fn                          \
    if (inst->cfg.exit)         \
    {                           \
        inst->cfg.exit();       \
    }                           \
} while (0U)
#define POLL_WAIT(inst)                                 \
do                                                      \
{                                                       \
    uint8_t wait = 1U;                                  \
    ATOMIC_ACCESS(inst, if (!inst->wait) {wait = 0;});  \
    if (wait == 0) {break;}                             \
} while (1U)

static inline void g_WriteEnable(GD25Q80C_Inst_t *inst);
static inline void g_TransferInProgress(GD25Q80C_Inst_t *inst);
static GD25Q80C_Status_t g_Wait(GD25Q80C_Inst_t *inst);

GD25Q80C_Status_t GD25Q80C_Init(GD25Q80C_Inst_t *inst, GD25Q80C_Config_t cfg)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;
    if (inst && cfg.transfer && cfg.delay)
    {
        status = GD25Q80C_OK;
        inst->cfg.transfer = cfg.transfer;
        inst->cfg.delay = cfg.delay;
        inst->wait = GD25Q80C_TRANSFER_COMPLETE;
        if (cfg.enter && cfg.exit)
        {
            inst->cfg.enter = cfg.enter;
            inst->cfg.exit = cfg.exit;
        }
    }
    return status;
}

GD25Q80C_Status_t GD25Q80C_Program(GD25Q80C_Inst_t *inst,
                                     uint32_t address,
                                     uint8_t *data,
                                     uint16_t size)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;
    uint8_t cmd[GD25Q80C_CMD_PROG_LENGTH] =
    {
        GD25Q80C_CMD_PROG,
        GET_BITS_23_16(address),
        GET_BITS_15_8(address),
        GET_BITS_7_0(address),
    };
    GD25Q80C_Data_t tx = {0U};
    ASSIGN_DATA_STRUCT(tx,
                       cmd,
                       GD25Q80C_CMD_READ_LENGTH,
                       data,
                       NULL,
                       size);

    if (inst && data && size && (address + size < GD25Q80C_CHIP_SIZE))
    {
        if (((address % GD25Q80C_PAGE_SIZE) + size <= GD25Q80C_PAGE_SIZE))
        {
            status = g_Wait(inst);
            if (status == GD25Q80C_OK)
            {
                g_WriteEnable(inst);
                g_TransferInProgress(inst);
                status = inst->cfg.transfer(tx);
            }
        }
        else
        {
            status = GD25Q80C_EXCEED_BOUNDRY;
        }
    }

    return status;
}

GD25Q80C_Status_t GD25Q80C_Read(GD25Q80C_Inst_t *inst,
                                    uint32_t address,
                                    uint8_t *data,
                                    uint16_t size)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;
    uint8_t cmd[GD25Q80C_CMD_READ_LENGTH] =
    {
        GD25Q80C_CMD_READ,
        GET_BITS_23_16(address),
        GET_BITS_15_8(address),
        GET_BITS_7_0(address),
    };
    GD25Q80C_Data_t rx = {0U};
    ASSIGN_DATA_STRUCT(rx,
                       cmd,
                       GD25Q80C_CMD_READ_LENGTH,
                       NULL,
                       data,
                       size);

    if (inst && data && size && (address + size < GD25Q80C_CHIP_SIZE))
    {
        status = g_Wait(inst);
        if (status == GD25Q80C_OK)
        {
            g_TransferInProgress(inst);
            status = inst->cfg.transfer(rx);
        }
    }

    return status;
}

GD25Q80C_Status_t GD25Q80C_EraseSector(GD25Q80C_Inst_t *inst, uint16_t sector)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;
    uint32_t address = sector * GD25Q80C_SECTOR_SIZE;
    uint8_t cmd[GD25Q80C_CMD_ERASE_SECTOR_LENGTH] =
    {
        GD25Q80C_CMD_ERASE_SECTOR,
        GET_BITS_23_16(address),
        GET_BITS_15_8(address),
        GET_BITS_7_0(address),
    };
    GD25Q80C_Data_t tx = {0U};
    ASSIGN_DATA_STRUCT(tx,
                       cmd,
                       GD25Q80C_CMD_ERASE_SECTOR_LENGTH,
                       NULL,
                       NULL,
                       0U);

    if (inst && address < GD25Q80C_CHIP_SIZE)
    {
        status = g_Wait(inst);
        if (status == GD25Q80C_OK)
        {
            g_WriteEnable(inst);
            g_TransferInProgress(inst);
            status = inst->cfg.transfer(tx);
        }
    }

    return status;
}

GD25Q80C_Status_t GD25Q80C_EraseChip(GD25Q80C_Inst_t *inst)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;
    uint8_t cmd[GD25Q80C_CMD_ERASE_CHIP_LENGTH] =
    {
        GD25Q80C_CMD_ERASE_CHIP,
    };
    GD25Q80C_Data_t tx = {0U};
    ASSIGN_DATA_STRUCT(tx,
                       cmd,
                       GD25Q80C_CMD_ERASE_CHIP_LENGTH,
                       NULL,
                       NULL,
                       0U);

    if (inst)
    {
        status = g_Wait(inst);
        if (status == GD25Q80C_OK)
        {
            g_WriteEnable(inst);
            g_TransferInProgress(inst);
            status = inst->cfg.transfer(tx);
        }
    }

    return status;
}

GD25Q80C_Status_t GD25Q80C_Reset(GD25Q80C_Inst_t *inst)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;
    uint8_t enable[GD25Q80C_CMD_ENABLE_RESET_LENGTH] =
    {
        GD25Q80C_CMD_ENABLE_RESET,
    };
    uint8_t reset[GD25Q80C_CMD_RESET_LENGTH] =
    {
        GD25Q80C_CMD_RESET,
    };
    GD25Q80C_Data_t tx = {0U};

    if (inst)
    {
        status = g_Wait(inst);
        if (status == GD25Q80C_OK)
        {
            ASSIGN_DATA_STRUCT(tx, enable, GD25Q80C_CMD_ENABLE_RESET_LENGTH,
                    NULL, NULL, 0U);
            status = inst->cfg.transfer(tx);
        }
        if (status == GD25Q80C_OK)
        {
            POLL_WAIT(inst);
            ASSIGN_DATA_STRUCT(tx, reset, GD25Q80C_CMD_RESET_LENGTH,
                               NULL, NULL, 0U);
            status = inst->cfg.transfer(tx);
        }
        if (status == GD25Q80C_OK)
        {
            inst->cfg.delay(RESET_DELAY_US);
        }
    }

    return status;
}

GD25Q80C_Status_t GD25Q80C_Sleep(GD25Q80C_Inst_t *inst)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;
    uint8_t cmd[GD25Q80C_DEEP_POWER_DOWN_LENGTH] =
    {
        GD25Q80C_DEEP_POWER_DOWN,
    };
    GD25Q80C_Data_t tx = {0U};

    if (inst)
    {
        status = g_Wait(inst);
        if (status == GD25Q80C_OK)
        {
            ASSIGN_DATA_STRUCT(tx, cmd, GD25Q80C_DEEP_POWER_DOWN_LENGTH,
                               NULL, NULL, 0U);
            status = inst->cfg.transfer(tx);
        }
    }

    return status;
}

GD25Q80C_Status_t GD25Q80C_WakeUp(GD25Q80C_Inst_t *inst)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;
    uint8_t cmd[GD25Q80C_RELEASE_DEEP_POWER_DOWN_LENGTH] =
    {
        GD25Q80C_RELEASE_DEEP_POWER_DOWN,
    };
    GD25Q80C_Data_t tx = {0U};

    if (inst)
    {
        status = g_Wait(inst);
        if (status == GD25Q80C_OK)
        {
            ASSIGN_DATA_STRUCT(tx, cmd, GD25Q80C_DEEP_POWER_DOWN_LENGTH,
                               NULL, NULL, 0U);
            status = inst->cfg.transfer(tx);
        }
    }

    return status;
}

void GD25Q80C_TransferComplete(GD25Q80C_Inst_t *inst)
{
    if (inst)
    {
        inst->wait = GD25Q80C_TRANSFER_COMPLETE;
    }
}

GD25Q80C_Status_t GD25Q80C_GetTransferStatus(GD25Q80C_Inst_t *inst)
{
    GD25Q80C_Status_t status = GD25Q80C_INVALID_PARAM;

    if (inst)
    {
        status = GD25Q80C_OK;
        ATOMIC_ACCESS(inst,
                      if (inst->wait) {status = GD25Q80C_TRANSFER_ONGOING;});
    }
    
    return status;
}

static inline void g_WriteEnable(GD25Q80C_Inst_t *inst)
{
    uint8_t cmd[GD25Q80C_CMD_WEL_LENGTH] =
    {
        GD25Q80C_CMD_WEL
    };
    GD25Q80C_Data_t tx = {0U};
    ASSIGN_DATA_STRUCT(tx, cmd, GD25Q80C_CMD_WEL_LENGTH, NULL, NULL, 0U);
    g_TransferInProgress(inst);
    
    inst->cfg.transfer(tx);
    POLL_WAIT(inst);
}

static inline void g_TransferInProgress(GD25Q80C_Inst_t *inst)
{
    inst->wait = GD25Q80C_TRANSFER_INPROGRESS;
}

GD25Q80C_Status_t g_Wait(GD25Q80C_Inst_t *inst)
{
    GD25Q80C_Status_t status = GD25Q80C_OK;
    uint8_t cmd[GD25Q80C_CMD_STATUS_LENGTH] = {GD25Q80C_CMD_READ_STATUS};
    uint8_t reg[GD25Q80C_CMD_STATUS_LENGTH +
                GD25Q80C_NUM_STATUS_REGISTERS] = {0U};
    uint32_t waitCnt = 0U;
    GD25Q80C_Data_t rx = {0U};
    ASSIGN_DATA_STRUCT(rx,
                       cmd,
                       GD25Q80C_CMD_STATUS_LENGTH,
                       NULL,
                       reg,
                       GD25Q80C_CMD_STATUS_LENGTH +
                       GD25Q80C_NUM_STATUS_REGISTERS);

    reg[GD25Q80C_CMD_STATUS_LENGTH] = GD25Q80C_STATUS_WIP_BIT |
                                      GD25Q80C_STATUS_WEL_BIT;

    while (reg[GD25Q80C_CMD_STATUS_LENGTH] & GD25Q80C_STATUS_WIP_BIT ||
           reg[GD25Q80C_CMD_STATUS_LENGTH] & GD25Q80C_STATUS_WEL_BIT)
    {
        waitCnt++;
        POLL_WAIT(inst);
        g_TransferInProgress(inst);
        inst->cfg.transfer(rx);
        if (waitCnt == UINT32_MAX)
        {
            status = GD25Q80C_TIMEOUT;
            break;
        }
    }

    return status;
}

/**@} GD25Q80C */
