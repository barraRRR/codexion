/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 10:45:52 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/22 11:37:40 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    fifo_scheduler(t_coder **queue, t_coder *coder)
{
    if (!queue[0])
        queue[0] = coder;
    else
        queue[1] = coder;
}

void    edf_scheduler(t_coder **queue, t_coder *coder)
{
    if (!queue[0])
        queue[0] = coder;
    else if (coder->sim->time_to_burnout < queue[0]->sim->time_to_burnout)
    {
        queue[1] = queue[0];
        queue[0] = coder;
    }
    else
        queue[1] = coder;
}

t_coder *dequeue(t_coder **queue)
{
    t_coder             *ptr;

    ptr = queue[0];
    queue[0] = queue[1];
    queue[1] = NULL;
    return (ptr);
}

void    enqueue_coder(t_coder *coder)
{
    if (coder->sim->scheduler == FIFO)
    {
        fifo_scheduler(coder->usb_right->queue, coder);
        fifo_scheduler(coder->usb_left->queue, coder);
    }
    else
    {
        edf_scheduler(coder->usb_right->queue, coder);
        edf_scheduler(coder->usb_left->queue, coder);
    }
    coder->status = WAITING_DONGLE;
}
