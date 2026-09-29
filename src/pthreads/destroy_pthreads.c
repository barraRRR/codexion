/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_pthreads.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 16:59:58 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/29 12:55:43 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	pthread_destroy_coder(t_coder *coder)
{
	if (coder->has_thread)
		pthread_join(coder->thread, NULL);
	if (coder->has_lock)
		pthread_mutex_destroy(&coder->lock);
}

void	pthread_destroy_dongle(t_dongle *dongle)
{
	if (dongle->has_lock)
		pthread_mutex_destroy(&dongle->lock);
	if (dongle->has_cond)
		pthread_cond_destroy(&dongle->cond);
}

void	pthread_destroy_monitor(t_monitor *monitor)
{
	if (monitor->has_thread)
		pthread_join(monitor->thread, NULL);
}

void	pthread_destroy_printer(t_printer *printer)
{
	if (printer->has_lock)
	{
		pthread_mutex_lock(&printer->lock);
		printer->stop = true;
		pthread_cond_broadcast(&printer->cond);
		pthread_mutex_unlock(&printer->lock);
	}
	if (printer->has_thread)
		pthread_join(printer->thread, NULL);
	if (printer->has_lock)
		pthread_mutex_destroy(&printer->lock);
	if (printer->has_cond)
		pthread_cond_destroy(&printer->cond);
}

void	pthread_destroy_sim(t_simulation *sim)
{
	if (sim->has_lock)
		pthread_mutex_destroy(&sim->lock);
	if (sim->has_start_lock)
		pthread_mutex_destroy(&sim->start_lock);
	if (sim->has_start_cond)
		pthread_cond_destroy(&sim->start_cond);
}
