/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 10:45:52 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/22 17:32:24 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	fifo_scheduler(t_coder **queue, t_coder *coder)
{
	if (!queue[0])
		queue[0] = coder;
	else
		queue[1] = coder;
}

void	edf_scheduler(t_coder **queue, t_coder *coder)
{
	if (!queue || !coder)
		return ;
	if (!queue[0])
		queue[0] = coder;
	else if (coder->last_compile_start < queue[0]->last_compile_start)
	{
		queue[1] = queue[0];
		queue[0] = coder;
	}
	else if (coder->last_compile_start == queue[0]->last_compile_start)
	{
		if (coder->id <= queue[0]->id)
		{
			queue[1] = queue[0];
			queue[0] = coder;
		}
		else
			queue[1] = coder;
	}
	else if (!queue[1])
		queue[1] = coder;
}

t_coder	*dequeue(t_coder **queue)
{
	t_coder				*ptr;

	ptr = queue[0];
	queue[0] = queue[1];
	queue[1] = NULL;
	return (ptr);
}

void	enqueue_coder(t_coder *coder)
{
	lock_dongles_in_order(coder);
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
	unlock_dongles(coder);
	if (!am_i_burnt(coder))
		coder->status = WAITING_DONGLE;
	pthread_mutex_unlock(&coder->lock);
}
