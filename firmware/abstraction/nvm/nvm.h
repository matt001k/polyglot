/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_NVM_H
#define __BL_NVM_H

/**
 * @addtogroup nvm
 * @{
 */

/**************************************************************************//**
 * @file        nvm.h
 *
 * @brief       Provides an abstraction layer for the bootloader's Non-Volatile
 *              memory storage. This can be anything from internal flash or
 *              external EEPROM/flash etc.
 *
 * @see         config.h explains how to configure the NVM nodes
 *
 * @author      Matthew Krause
 *
 * @date        2022-10-03
 *****************************************************************************/
#include "config.h"

#define NVM_COUNTER(init, write, read, erase, size, \
                    location, sector_size, partition) \
    NVM_NODE_##partition,
#define PARTITION_NODE (0U)
#define APPLICATION_NODE (1U)

typedef void (*NVM_Init_t)(void);
typedef BL_BOOL_T (*NVM_Write_t)(BL_UINT32_T address,
                                 BL_UINT8_T *data,
                                 BL_UINT32_T length);
typedef BL_BOOL_T (*NVM_Read_t)(BL_UINT32_T address,
                                BL_UINT8_T *data,
                                BL_UINT32_T length);
typedef BL_BOOL_T (*NVM_Erase_t)(BL_UINT32_T address,
                                 BL_UINT32_T size);

enum
{
    NVM_CFG(NVM_COUNTER)
    NUM_NVM_NODES
};

/**************************************************************************//**
 * @brief Initialize The Configured NVM Peripherals
 *
 * @details This sorts the array of peripherals in ascending order, when higher
 *          level API wants to write to certain partitions it will begin with
 *          the lowest priority
 *
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_Init(void);

/**************************************************************************//**
 * @brief Write data to the NVM node
 *
 * @details Writes data to NVM node, the data must be erased first before a
 *          write in upper level API.
 *
 * @see NVM_Erase for erase definition
 *
 * @param node[in] node to write data to
 * @param data[in] data to write
 * @param length[in] length of data to write
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_Write(BL_UINT8_T node, BL_UINT8_T *data, BL_UINT32_T length);

/**************************************************************************//**
 * @brief Read Data from the NVM Node
 *
 * @details Obtains data from the requested node
 *
 * @param node[in] node to read data from
 * @param data[out] data read
 * @param length[in/out] length of data that is to be read/was read
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_Read(BL_UINT8_T node, BL_UINT8_T *data, BL_UINT32_T *length);

/**************************************************************************//**
 * @brief Erases the Requested Node
 *
 * @details Erases the requested node by length
 *
 * @param node[in] node to erase
 * @param length[in] length to erase from the node
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_Erase(BL_UINT8_T node, BL_UINT32_T length);

/**************************************************************************//**
 * @brief Finishes an Operation on The Requested Node
 *
 * @details Finishes an operation on a requested node and set the pointer for
 *          node operations back to the beginning of the nodes origin/location
 *
 * @param node[in] node to perform requested action
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_OperationFinish(BL_UINT8_T  node);

/**************************************************************************//**
 * @brief Obtains the Total Size of the Requested Node Partition
 *
 * @param node[in] node to obtain the size of
 * @param size[out] total size of the requested node
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_GetSize(BL_UINT8_T node, BL_UINT32_T *size);

/**************************************************************************//**
 * @brief Obtains Page Size from Requested Node
 *
 * @param node[in] node to obtain the page size from
 * @param size[out] page size from the requested node
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_GetPageSize(BL_UINT8_T node, BL_UINT32_T *size);

/**************************************************************************//**
 * @brief Obtains the Number Of Nodes Configured
 *
 * @see config.h for the number of nodes configured
 *
 * @param count[out] number of nodes configured
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_GetCount(BL_UINT8_T *count);

/**************************************************************************//**
 * @brief Obtain the Location from Requested Node
 *
 * @param node[in] node to obtain the requested beginning location
 * @param location[out] address of the requested node's partition
 * @return BL_Err_t
 *****************************************************************************/
BL_Err_t NVM_GetLocation(BL_UINT8_T node, BL_UINT32_T *location);

/**@} nvm */

#endif //__BL_NVM_H
