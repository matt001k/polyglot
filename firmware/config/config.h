/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup config
 * @{
 */

/**
 * @file        config.h
 *
 * @brief       Configuration file necessary for bootloader
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-14
 */

/*****************************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**************************************************************************//**
 * @brief Definitions of various types within the bootloader in accordance to
 *        system requirements
 *
 * @details All of these must be defined in order to use the bootloader
 *****************************************************************************/
#define BL_UINT8_T uint8_t
#define BL_UINT16_T uint16_t
#define BL_UINT32_T uint32_t
#define BL_UINT64_T uint64_t

#define BL_INT8_T int8_t
#define BL_INT16_T int16_t
#define BL_INT32_T int32_t
#define BL_INT64_T int64_t

#define BL_BOOL_T bool
#define BL_TRUE true
#define BL_FALSE false

#define BL_STATIC static
#define BL_INLINE inline
#define BL_CONST const

#define BL_SIZEOF sizeof

#define BL_NULL NULL

typedef enum
{
    BL_OK       = 0U,
    BL_ERR      = 1U,
    BL_ENOENT   = 2U,
    BL_EIO      = 5U,
    EL_ENXIO    = 6U,
    BL_ENOMEM   = 12U,
    BL_EACCES   = 13U,
    BL_EBUSY    = 16U,
    BL_ENODEV   = 19U,
    BL_EINVAL   = 22U,
    BL_ENOSYS   = 38U,
    BL_ENOMSG   = 41U,
    BL_ENODATA  = 61U,
    BL_EALREADY = 116U,
} BL_Err_t;

#define BL_BUFFER_SIZE (1024U)
#define BL_NUM_PARTITIONS_TO_UPDATE (2U)
#define BL_SERIAL_TIMEOUT_MS (20U)

/**************************************************************************//**
 * @brief Abstractions for Necessary Functions
 *
 * @details This ensures that the necessary function abstractions will be
 *          compatible with the API being ported.
 *****************************************************************************/

/**************************************************************************//**
 * @brief Configuration Entries for Serial Peripherals
 *
 * @details This configuration includes all of the serial peripherals that are
 *          planned on being utilized within the bootloader. Serial peripherals
 *          include anything that may speak serially, such as SPI, UART, I2C,
 *          CAN, LIN, etc. The correct format of an entry is as follows:
 *
 *          ENTRY(name, index, init, transmit, register, deregister)
 *
 *          @param name name of the serial module, this is text not a string
 *          @param index index of the entry, this starts at zero
 *          @param init initialization function pointer in the format of:
 *
 *                  void init(void)
 *
 *          @param transmit transmit function pointer to transmit data in the
 *                          format of:
 *
 *                          void transmit(BL_UINT8_T *data, BL_UINT32_T length)
 *
 *          @param register register function pointer to register a receive
 *                          interrupt in the format of:
 *
 *                          void register(void (*cb)(BL_UINT8_T *data,
 *                                        BL_UINT32_T length))
 *
 *          @param deregister deregister function pointer to deregister a
 *                            receive interrupt in the format of:
 *
 *                            void deregister(void)
 *
 *          The serial abstraction will lock onto the first peripheral to take
 *          ahold of the bus. Once that has been established another peripheral
 *          cannot take control unless a command is sent to the device to
 *          unlock the peripheral.
 *****************************************************************************/
#define SERIAL_CFG(ENTRY)                       \

/**************************************************************************//**
 * @brief Configuration Entry for Systick Peripheral
 *
 * @details This configuration is used for the systick timer which will be used
 *          for handling certain events through the bootloader process. Only
 *          one systick timer may be entered, if multiple timer's are
 *          entered, the system will throw an assertion. The correct format
 *          of an entry looks as follows:
 *
 *          ENTRY(init, ms)
 *
 *          @param init initialization function pointer in the format of:
 *
 *                      void init(void)
 *
 *          @param ms function pointer which obtains the current runtime of the
 *                    system in milliseconds in the format of:
 *
 *                    BL_UINT32_T ms(void)
 *
 *****************************************************************************/
#define SYSTICK_CFG(ENTRY)                      \

/**************************************************************************//**
 * @brief Configuration Entry for LED Peripheral
 *
 * @details This configuration is used for toggling LEDs while the bootloader
 *          is running. There can be up to 255 LEDs used, although it is not
 *          recommended or necessary. LED periods must be no less than 100ms,
 *          they should also be aligned to the nearest 100ms as that is how
 *          often the task for the LED is running, the led's will not
 *          run properly if this is not configured correctly. The correct
 *          format of an entry looks as follows:
 *
 *          ENTRY(toggle, period)
 *
 *          @param toggle toggle function pointer in the format of:
 *
 *                        void toggle(void)
 *
 *          @param period period of the led toggling
 *****************************************************************************/
#define LED_CFG(ENTRY)            \

/**************************************************************************//**
 * @brief Configuration Entry for Watchdog Timer
 *
 * @details This configuration is used for ensuring that the bootloader does
 *          not get stuck in an infinite loop. The WDT will reset the device.
 *          Only one watchdog timer can be utilized, if more than one are
 *          defined, an assertion will be thrown. The correct format of an
 *          entry looks as follows:
 *
 *          ENTRY(init, kick)
 *
 *          @param init initialization function for the WDT, this will include
 *                      API used to start the watchdog as well, the format of
 *                      this function is as follows:
 *
 *                      void init(void)
 *
 *          @param kick API to kick the watchdog and keep the bootloader alive
 *                      the format of this function is as follows:
 *
 *                      void kick(void)
 *
 *****************************************************************************/
#define WDT_CFG(ENTRY)                          \

/**************************************************************************//**
 * @brief Configuration Entry for NVM Partitions
 *
 * @details This configuration is used for ensuring that the bootloader will
 *          properly write to required partitions. The required partitions are
 *          as follows:
 *              0: Bootloader partition for requested bootloader data
 *              1: Main application partition
 *          All other partitions are secondary partitions to write to. These
 *          will store the older updates on the device. The correct format of
 *          an entry looks as follows:
 *
 *          ENTRY(init,
 *                write,
 *                read,
 *                erase,
 *                size,
 *                location,
 *                sector_size,
 *                partition)
 *
 *          @param init initialization function for the NVM partition, the
 *                      format of the function is as follows:
 *
 *                      void init(void)
 *
 *          @param write write function for the NVM partition, this function
 *                       requires an address to write to, a data buffer to
 *                       write from, and length of to write to. This returns
 *                       whether or not the write operation is finished or
 *                       needs to be polled again. The format of the function
 *                       is as follows:
 *
 *                       BL_BOOL_T write(BL_UINT32_T address,
 *                                       BL_UINT8_T *data,
 *                                       BL_UINT32_T length)
 *
 *          @param read read function from the NVM partition, this function
 *                      requires an address to read from, a data buffer to
 *                      read to, and a length to read to. This returns whether
 *                      or not the read operation is finished or needs to be
 *                      polled again. The format of the function is as
 *                      follows:
 *
 *                      BL_BOOL_T read(BL_UINT32_T address,
 *                                     BL_UINT8_T *data,
 *                                     BL_UINT32_T length)
 *
 *          @param erase erase function for the NVM partition, this function
 *                       requires an address to erase, and size of the
 *                       partition to erase. This returns whether or not the
 *                       erase operation is finished or needs to be polled
 *                       again. The format of the function is as follows:
 *
 *                       BL_BOOL_T erase(BL_UINT32_T address,
 *                                       BL_UINT32_T size)
 *
 *          @param size size of the partition in bytes
 *
 *          @param location address location in bytes of the partitions
 *
 *          @param sector_size size in bytes of the flash sectors within that
 *                              partition
 *
 *          @param partition partition number, see required partitions
 *
 *****************************************************************************/
#define NVM_CFG(ENTRY)                      \

/**************************************************************************//**
 * @brief Configuration Entry for Jump @details This peripheral is used to jump to a valid application in the main application partition. Only one jump entry can be configured at a time, if more than one is configured an assertion will be thrown. The correct format of an entry is as follows:
 *
 *          ENTRY(jump)
 *
 *          @param jump jump function to jump to application. The correct
 *                      format of the function is as follows:
 *
 *                      void jump(BL_UINT32_T address)
 *
 *****************************************************************************/
#define JUMP_CFG(ENTRY)             \

/**************************************************************************//**
 * @brief Configuration Entry for Hold
 *
 * @details This peripheral is used to hold the device in bootloader mode even
 *          if there is a valid application in the main application partition.
 *          Only one hold entry can be configured at a time, if more than one
 *          configured an assertion will be thrown. The correct format of an
 *          entry is as follows:
 *
 *          ENTRY(hold)
 *
 *          @param hold hold function to hold the device in bootloader mode.
 *                      The device will stay in bootloader mode if this
 *                      function returns BL_TRUE. The correct format of the
 *                      function is as follows:
 *
 *                      BL_BOOL_T hold(void)
 *
 *****************************************************************************/
#define HOLD_CFG(ENTRY)             \

/**************************************************************************//**
 * @brief Configuration for peripheral initialization
 *
 *****************************************************************************/
#define INIT_CFG(ENTRY)              \

/**************************************************************************//**
 * @brief Configuration Entry for AES
 *
 * @details This peripheral is used to decrypt incoming image data that would
 *          occur over a serial line. The peripheral uses AES-CBC which
 *          requires an initialization vector. The correct format of an entry
 *          is as follows:
 *
 *          ENTRY(key, iv, decrypt)
 *
 *          @param key function to get the key necessary for decrypting data.
 *                     The correct format of the function is as follows:
 *
 *                     BL_UINT8_T *key(void)
 *
 *                     This must return a key in size of the length of AES
 *                     decryption type (128 or 256 bit)
 *
 *          @param iv function to get the iv necessary for decrypting data.
 *                    The correct format of the function is as follows:
 *
 *                    BL_UINT8_T *iv(void)
 *
 *                    This must return a iv of 16 bytes
 *
 *          @param decrypt function that will decrypt new data received. The
 *                         correct format of the function is as follows:
 *
 *                         BL_BOOL_T decrypt(BL_UINT8_T *input,
 *                                           BL_UINT8_T *output,
 *                                           BL_UINT32_T size
 *                                           BL_UINT8_T *key
 *                                           BL_UINT8_T *iv)
 *
 *****************************************************************************/
#define AES_CFG(ENTRY)               \

/**************************************************************************//**
 * @brief Configuration Entry for SHA256
 *
 * @details This peripheral is used to calculate SHA256 hash checksum of
 *          firmware that is to be loaded onto the device. The methodology for
 *          obtaining the checksum consists of 3 functions, a start, update and
 *          finish function. The correct format of an entry is as follows:
 *
 *          ENTRY(start, update, finish)
 *
 *          @param start function to begin a SHA256 hashing process.
 *                       The correct format of the function is as follows:
 *
 *                       void *start(void)
 *
 *          @param update function to update the SHA256 hash in process
 *                        The correct format of the function is as follows:
 *
 *                        BL_BOOL update(BL_UINT8_T *data, BL_UINT32_T size)
 *
 *                        This function is non-blocking and must return true
 *                        once the process is complete.
 *
 *          @param finish function which will finish the SHA256 hashing. The
 *                        correct format of the function is as follows:
 *
 *                         BL_BOOL_T finish(BL_UINT8_T *digest)
 *
 *****************************************************************************/
#define SHA_CFG(ENTRY)               \

/**************************************************************************//**
 * @brief Configuration Entry for Signature Verification
 *
 * @details This peripheral is used to validate the hash checksum of the
 *          firmware to a signature. A key must be provided through a callback
 *          function, then a validation function is called with a hash and
 *          signature to verify. The correct format of an entry is as follows:
 *
 *          ENTRY(key, verify)
 *
 *          @param key function to get the key necessary for verifying data.
 *                     The correct format of the function is as follows:
 *
 *                     BL_UINT8_T *key(void)
 *
 *                     This function returns a pointer to a buffer the key will
 *                     be located in.
 *
 *          @param verify function used to validate the signature of a hash.
 *                        The correct format of the function is as follows:
 *
 *                        BL_BOOL_T verify(BL_UINT8_T *hash,
 *                                         BL_UINT8_T *signature,
 *                                         BL_UINT8_T *key)
 *
 *                        This function returns if the signature was verified.
 *
 *****************************************************************************/
#define VERIFY_CFG(ENTRY)        \

#endif // __CONFIG_H

/**@} config */
