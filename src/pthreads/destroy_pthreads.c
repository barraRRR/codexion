/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_pthreads.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 16:59:58 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/30 13:04:28 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	pthread_destroy_hub(t_simulation *sim)
{
	int					i;

	i = -1;
	while (++i < sim->n_coders_init && sim->hub && sim->hub[i])
	{
		if (sim->hub[i]->has_thread)
			pthread_join(sim->hub[i]->thread, NULL);
		if (sim->hub[i]->has_lock)
			pthread_mutex_destroy(&sim->hub[i]->lock);
	}
}

void	pthread_destroy_dongle(t_simulation *sim)
{
	int					i;

	i = -1;
	while (++i < sim->n_dongles_init && sim->quantum && sim->quantum[i])
	{
		if (sim->quantum[i]->has_lock)
			pthread_mutex_destroy(&sim->quantum[i]->lock);
		if (sim->quantum[i]->has_cond)
			pthread_cond_destroy(&sim->quantum[i]->cond);
	}
}

void	pthread_destroy_monitor(t_monitor *monitor)
{
	int					i;

	if (monitor->has_thread)
		pthread_join(monitor->thread, NULL);
	i = -1;
	while (++i < monitor->n_sub)
	{
		pthread_join(monitor->pool[i]->thread, NULL);
		if (monitor->pool[i]->has_lock)
			pthread_mutex_destroy(&monitor->pool[i]->lock);
	}
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
