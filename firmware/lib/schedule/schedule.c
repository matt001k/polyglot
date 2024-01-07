/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup schedule
 * @{
 */

/**************************************************************************//**
 * @file        schedule.c
 *
 * @brief       API required for adding tasks to the scheduler
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-24
 *****************************************************************************/
#include "schedule.h"

typedef struct
{
    Schedule_Node_t *head;
    Schedule_Node_t *tail;
} schedule_t;

BL_STATIC schedule_t schedule = {0U};

BL_Err_t Schedule_Add(Schedule_Node_t *node,
                      BL_UINT32_T period,
                      void (*run)(void))
{
    BL_Err_t err = BL_EINVAL;

    if (node && period && run)
    {
        node->next = BL_NULL;
        node->period = period;
        node->run = run;

        /* If a tail exists, set the next pointer to this node and assign the
         * tail to this node. If a tail does not exist this is the head node */
        if (schedule.tail)
        {
            schedule.tail->next = node;
            schedule.tail = node;
        }
        else
        {
            schedule.head = node;
            schedule.tail = node;
        }

        err = BL_OK;
    }

    return err;
}

BL_Err_t Schedule_Remove(Schedule_Node_t *node)
{
    BL_Err_t err = BL_EINVAL;
    Schedule_Node_t *pNode = BL_NULL;

    if (node)
    {
        err = BL_ENODEV;
        if (node == schedule.head)
        {
            schedule.head = node->next;
            if (node == schedule.tail)
            {
                schedule.tail = BL_NULL;
            }
            err = BL_OK;
        }
        else
        {
            if (Schedule_GetHead(&pNode) == BL_OK)
            {
                while (pNode->next != BL_NULL)
                {
                    if (pNode->next == node)
                    {
                        if (node == schedule.tail)
                        {
                            pNode->next = NULL;
                        }
                        else
                        {
                            pNode->next = node->next;
                        }
                        err = BL_OK;
                        break;
                    }
                    pNode = pNode->next;
                }
            }
        }
    }

    return err;
}

BL_Err_t Schedule_GetHead(Schedule_Node_t **node)
{
    BL_Err_t err = BL_ENODEV;

    if (schedule.head)
    {
        *node = schedule.head;
        err = BL_OK;
    }

    return err;
}

BL_Err_t Schedule_GetNext(Schedule_Node_t current, Schedule_Node_t **next)
{
    BL_Err_t err = BL_ENODEV;

    if (next)
    {
        *next = current.next;
        err = BL_OK;
    }

    return err;
}

BL_Err_t Schedule_Run(Schedule_Node_t *node, BL_UINT32_T time)
{
    BL_Err_t err = BL_EINVAL;

    if (node)
    {
        err = BL_OK;

        if (time >= node->ms + node->period)
        {
            if (node->run)
            {
                node->run();
            }
            else
            {
                err = BL_ENODEV;
            }
            node->ms = time;
        }
    }

    return err;
}

/**@} schedule */
