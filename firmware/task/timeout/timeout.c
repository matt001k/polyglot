/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup timeout
 * @{
 */

/**************************************************************************//**
 * @file        timeout.c
 *
 * @brief       Task to determine certain timeouts defined by the user
 * 
 * @author      Matthew Krause
 *
 * @date        2022-11-06
 *****************************************************************************/
#include "timeout.h"
#include "schedule.h"

#define TIMEOUT_PERIOD_MS (1U)

BL_STATIC struct
{
    Timeout_Node_t *head;
    Timeout_Node_t *tail;
    Timeout_Node_t *read;
    BL_UINT32_T time;
} timeout = {0};

BL_STATIC void timeout_Run(void);

BL_Err_t Timeout_Init(void)
{
    BL_Err_t err = BL_ENODEV;
    BL_STATIC Schedule_Node_t node = {0};

    err = Schedule_Add(&node, TIMEOUT_PERIOD_MS, timeout_Run);

    return err;
}

BL_Err_t Timeout_Add(Timeout_Node_t *node,
                     BL_CONST Timeout_Cb_t *cb,
                     BL_UINT32_T period)
{
    BL_Err_t err = BL_EINVAL;

    if (node && period && cb)
    {
        node->ms = timeout.time;
        node->next = BL_NULL;
        node->period = period;
        node->cb = cb;

        /* If a tail exists, set the next pointer to this node and assign the
         * tail to this node. If a tail does not exist this is the head node */
        if (timeout.tail)
        {
            timeout.tail->next = node;
            timeout.tail = node;
        }
        else
        {
            timeout.head = node;
            timeout.tail = node;
        }

        err = BL_OK;
    }

    return err;
}

BL_Err_t Timeout_Remove(Timeout_Node_t *node)
{
    BL_Err_t err = BL_EINVAL;
    Timeout_Node_t *pNode = BL_NULL;

    if (node)
    {
        err = BL_ENODEV;
        if (node == timeout.head)
        {
            timeout.head = node->next;
            if (node == timeout.tail)
            {
                timeout.tail = BL_NULL;
            }
            err = BL_OK;
        }
        else
        {
            if ((pNode = timeout.head) != BL_NULL) 
            {
                while (pNode->next != BL_NULL)
                {
                    if (pNode->next == node)
                    {
                        if (node == timeout.tail)
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

BL_Err_t Timeout_Kick(Timeout_Node_t *node)
{
    BL_Err_t err = BL_EINVAL;

    if (node)
    {
        node->ms = timeout.time;
        err = BL_OK;
    }

    return err;
}

BL_STATIC void timeout_Run(void)
{
    Timeout_Node_t *node = timeout.head;
    BL_UINT8_T cbIdx = 0U;

    while (node != BL_NULL)
    {
        cbIdx = 0U;
        if (timeout.time - node->ms > node->period)
        {
            while (node->cb[cbIdx] != BL_NULL)
            {
                node->cb[cbIdx++]();
            }
        }
        node = node->next;
    }
    timeout.time += TIMEOUT_PERIOD_MS;
}

/**@} timeout */

