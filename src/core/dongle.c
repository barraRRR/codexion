/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 10:26:13 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/15 19:07:27 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	usb_access(t_dongle *usb, t_coder *coder)
{
	if (usb->status == PLUGGED)
		return (false);
	if (usb->status == COOLING_DOWN)
		return (false);
	if (usb->queue[0] && usb->queue[0] != coder)
		return (false);
	return (true);
}

t_status	both_usb_access(t_coder *coder)
{
	bool				right;
	bool				left;

	right = usb_access(coder->usb_right, coder);
	left = usb_access(coder->usb_left, coder);
	if (right && left)
		return (AVAILABLE_BOTH);
	else if (right)
		return (AVAILABLE_RIGHT);
	else if (left)
		return (AVAILABLE_LEFT);
	else
		return (AVAILABLE_NONE);
}

void	lock_dongles_in_order(t_coder *coder)
{
	if (coder->usb_right->id <= coder->usb_left->id)
	{
		pthread_mutex_lock(&coder->usb_right->lock);
		pthread_mutex_lock(&coder->usb_left->lock);
	}
	else
	{
		pthread_mutex_lock(&coder->usb_left->lock);
		pthread_mutex_lock(&coder->usb_right->lock);
	}
}

void	unlock_dongles(t_coder *coder)
{
	if (coder->usb_right)
		pthread_cond_broadcast(&coder->usb_right->cond);
	if (coder->usb_left)
		pthread_cond_broadcast(&coder->usb_left->cond);
	if (coder->usb_right)
		pthread_mutex_unlock(&coder->usb_right->lock);
	if (coder->usb_right)
		pthread_mutex_unlock(&coder->usb_left->lock);
}
