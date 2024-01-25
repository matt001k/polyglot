/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_TIMEOUT_H
#define __BL_TIMEOUT_H

/**
 * @addtogroup timeout
 * @{
 */

/**************************************************************************//**
 * @file        timeout.h
 *
 * @brief       Task to determine certain timeouts defined by the user
 * 
 * @author      Matthew Krause
 *
 * @date        2022-11-06
 *****************************************************************************/
#include "config.h"

typedef void (*Timeout_Cb_t)(void);

typedef struct Timeout_Node_s
{
    BL_UINT32_T ms;
    BL_UINT32_T period;
    struct Timeout_Node_s *next;
    BL_CONST Timeout_Cb_t *cb;
} Timeout_Node_t;


/**************************************************************************//**
 * @brief Initialize the Timeout Task
 * 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Timeout_Init(void);

/**************************************************************************//**
 * @brief Adds a node to timeout check
 * 
 * @note cb must be a BL_NULL terminated array
 * 
 * @param node[in/out] node to be added to timeout check
 * @param cb[in] array of callbacks to call when timeout is reached (see note)
 * @param period[in] period for the timeout to occur
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Timeout_Add(Timeout_Node_t *node,
                     BL_CONST Timeout_Cb_t *cb,
                     BL_UINT32_T period);

/**************************************************************************//**
 * @brief Remove a node from the timeout check 
 * 
 * @param node[in/out] node to be removed from the timeout check
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Timeout_Remove(Timeout_Node_t *node);

/**************************************************************************//**
 * @brief Resets the timeout for the required node 
 * 
 * @param node[in/out] node to be reset
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Timeout_Kick(Timeout_Node_t *node);
/**@} timeout */

#endif //__BL_TIMEOUT_H
