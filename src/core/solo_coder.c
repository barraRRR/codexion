/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solo_coder.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 19:15:29 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/15 19:15:48 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	solo_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->usb_right->lock);
	coder->usb_right->status = PLUGGED;
	pthread_mutex_unlock(&coder->usb_right->lock);
	pthread_mutex_lock(&coder->lock);
	coder->status = TAKING_DONGLE;
	print_log(coder, timer(&coder->sim->start), false);
	pthread_mutex_unlock(&coder->lock);
	while (!sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
		usleep(SLEEP_INTERVAL);
}
