/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup serial
 * @{
 */

/**
 * @file        serial.c
 *
 * @brief       Provides an abstraction layer for the bootloader's serial
 *              ports, which can be anything from a UART port, CAN bus,
 *              SPI bus, I2C bus, etc.
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-19
 */

/*****************************************************************************/
#include "serial.h"
#include "helper.h"

#define SERIAL_UNLOCKED (-1)

typedef struct
{
    BL_UINT8_T index;               ///< Name of the port
    Serial_Init_t init;             ///< Function pointer to initialization
    Serial_Transmit_t transmit;     ///< Function pointer to transmit
    Serial_RegisterCb_t reg;        ///< Function pointer to register cb
    Serial_DeregisterCb_t dereg;    ///< Function pointer to deregister cb
} serial_Cfg_t;

typedef struct
{
    BL_CONST serial_Cfg_t *cfg;     ///< Pointer to configuration
    BL_UINT8_T count;               ///< Number of serial ports
    BL_INT8_T lock;                 ///< Serial port to lock
    BL_UINT8_T buf[BL_BUFFER_SIZE]; ///< Serial buffer
    BL_UINT32_T bufIdx;             ///< Serial buffer index
    void (*cb)(BL_UINT32_T length); ///< Serial Callback
} serial_t;

BL_STATIC void serial_CbInit(BL_Err_t *err);
BL_STATIC BL_BOOL_T serial_LockCb(BL_UINT8_T lIdx);

#define SERIAL_ENTRY(name, index, init, tx, register, deregister) \
BL_STATIC void name##_Cb(BL_UINT8_T *data, BL_UINT32_T length);
    SERIAL_CFG
#undef SERIAL_ENTRY

/* NULL terminated config struct to accept multiple serial peripherals */
BL_STATIC BL_CONST serial_Cfg_t sCfg[] =
{
#define SERIAL_ENTRY(name, index, init, tx, register, deregister) \
    {index, init, tx, register, deregister},
    SERIAL_CFG
#undef SERIAL_ENTRY
    {0, 0, 0, 0, 0},
};

BL_STATIC serial_t serial = {0U};

BL_Err_t Serial_Init(void)
{
    BL_Err_t err = BL_OK;

    serial.cfg = sCfg;
    serial.lock = SERIAL_UNLOCKED;
    while (serial.cfg[serial.count].init != 0 &&
           serial.cfg[serial.count].transmit != 0 &&
           serial.cfg[serial.count].reg != 0 &&
           serial.cfg[serial.count].dereg != 0)
    {
        if (serial.cfg[serial.count].init)
        {
            serial.cfg[serial.count].init();
            serial.count++;
        }
        else
        {
            err = BL_EINVAL;
            break;
        }
    }

    serial_CbInit(&err);

    return err;
}

void Serial_Flush(void)
{
    if (serial.bufIdx > 0U)
    {
        MEMSET(serial.buf, 0U, BL_BUFFER_SIZE);
        serial.bufIdx = 0U;
    }
}

BL_Err_t Serial_Transmit(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = BL_EINVAL;

    if (serial.lock != SERIAL_UNLOCKED)
    {
        if (data && serial.cfg[serial.lock].transmit && length)
        {
            serial.cfg[serial.lock].transmit(data, length);
            err = BL_OK;
        }
    }
    else
    {
        err = BL_ENODEV;
    }

    return err;
}

BL_Err_t Serial_Receive(BL_UINT8_T *data, BL_UINT32_T length)
{
    BL_Err_t err = BL_EINVAL;

    if (data && length)
    {
        /* Assign rx buffer to data, then flush the rx buffer */
        MEMCPY(data, serial.buf, length);
        MEMSET(serial.buf, 0U, BL_BUFFER_SIZE);
        serial.bufIdx = 0U;
    }

    return err;
}

BL_Err_t Serial_RegisterCb(void (*cb)(BL_UINT32_T length))
{
    BL_Err_t err = BL_EINVAL;

    if (cb)
    {
        serial.cb = cb;
        err = BL_OK;
    }

    return err;
}

BL_Err_t Serial_DeregisterCb(void)
{
    BL_Err_t err = BL_EACCES;

    if (serial.cb)
    {
        serial.cb = BL_NULL;
        err = BL_OK;
    }

    return err;
}

BL_Err_t Serial_Unlock(void)
{
    BL_Err_t err = BL_OK;

    serial.lock = SERIAL_UNLOCKED;
    serial_CbInit(&err);

    return BL_OK;
}

#define SERIAL_ENTRY(name, index, init, tx, register, deregister)   \
BL_STATIC void name##_Cb(BL_UINT8_T *data, BL_UINT32_T length)      \
{                                                                   \
    if (serial_LockCb(index))                                       \
    {                                                               \
        if (serial.cb)                                              \
        {                                                           \
            serial.cb(length);                                      \
        }                                                           \
        while (length--)                                            \
        {                                                           \
            if (serial.bufIdx < BL_BUFFER_SIZE)                     \
            {                                                       \
                serial.buf[serial.bufIdx++] = *data++;              \
            }                                                       \
        }                                                           \
    }                                                               \
}
    SERIAL_CFG
#undef SERIAL_ENTRY

BL_STATIC void serial_CbInit(BL_Err_t *err)
{
#define SERIAL_ENTRY(name, index, init, tx, register, deregister) \
    if (serial.cfg[index].reg && *err == BL_OK)                   \
    {                                                             \
        serial.cfg[index].reg(name##_Cb);                         \
    }                                                             \
    else                                                          \
    {                                                             \
        *err = BL_EINVAL;                                         \
    }
    SERIAL_CFG
#undef SERIAL_ENTRY
}

BL_STATIC BL_BOOL_T serial_LockCb(BL_UINT8_T lIdx)
{
    bool ret = true;

    /* If the module is not locked, lock this peripheral */
    if (serial.lock == SERIAL_UNLOCKED)
    {
        serial.lock = (BL_INT8_T) lIdx;

        /* Deregister any rx callbacks not being used */
#define SERIAL_ENTRY(name, index, init, tx, register, deregister) \
        if (serial.cfg[index].dereg && serial.lock != index)      \
        {                                                         \
            serial.cfg[index].dereg();                            \
        }
    SERIAL_CFG
#undef SERIAL_ENTRY

    }
    else if (lIdx != serial.lock)
    {
        ret = false;
    }

    return ret;
}

/**@} serial */