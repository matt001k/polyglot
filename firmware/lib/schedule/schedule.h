/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_SCHEDULE_H
#define __BL_SCHEDULE_H

/**
 * @addtogroup schedule
 * @{
 */

/**************************************************************************//**
 * @file        schedule.h
 *
 * @brief       API required for adding tasks to the scheduler
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-24
 *****************************************************************************/
#include "config.h"

typedef struct Schedule_Node_s
{
    BL_UINT32_T ms;
    BL_UINT32_T period;
    struct Schedule_Node_s *next;
    void (*run)(void);
} Schedule_Node_t;

/**************************************************************************//**
 * @brief Adds an item to the schedule module
 * 
 * @param node[in] node to add, this must be a statically allocated variable
 * @param period[in] period in ms that the task will run
 * @param run[in] function pointer to task run function
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Schedule_Add(Schedule_Node_t *node,
                      BL_UINT32_T period,
                      void (*run)(void));

/**************************************************************************//**
 * @brief Remove a task from the scheduler
 * 
 * @param node[in] node to remove from the scheduler
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Schedule_Remove(Schedule_Node_t *node);

/**************************************************************************//**
 * @brief Obtains the head node of the schedule module
 * 
 * @details This will reset the current read node of the module to the head, so
 *          that when running Schedule_GetNext, it begins dequeueing the head
 * 
 * @param node 
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Schedule_GetHead(Schedule_Node_t **node);

/**************************************************************************//**
 * @brief Get the next node in the schedule module
 * 
 * @param current[in] current node
 * @param next[out] next node to be obtained
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Schedule_GetNext(Schedule_Node_t current, Schedule_Node_t **next);

/**************************************************************************//**
 * @brief Run the node requested
 * 
 * @param node node to run
 * @param ms current run time in ms
 * @return BL_Err_t 
 *****************************************************************************/
BL_Err_t Schedule_Run(Schedule_Node_t *node, BL_UINT32_T time);

/**@} schedule */

#endif //__BL_SCHEDULE_H