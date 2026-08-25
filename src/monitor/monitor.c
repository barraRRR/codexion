/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:52:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/25 13:58:15 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	update_cooldown(t_dongle *usb, struct timeval *start)
{
	long long			now;

	pthread_mutex_lock(&usb->lock);
	if (usb->status == COOLING_DOWN)
	{
		now = timer(start);
		if (now >= usb->last_compile_time + usb->sim->dongle_cooldown)
		{
			usb->status = AVAILABLE;
			pthread_cond_broadcast(&usb->cond);
		}
	}
	pthread_mutex_unlock(&usb->lock);
}

t_status	check_coder_status_and_cooldown(t_simulation *sim)
{
	int					i;
	long long			last_compile;
	long long			now;
	t_status			status;

	i = -1;
	status = SHUTDOWN_SIGNAL;
	while (++i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->hub[i]->lock);
		last_compile = sim->hub[i]->last_compile_time;
		now = timer(&sim->start);
		if (now >= last_compile + sim->time_to_burnout)
		{
			sim->hub[i]->status = BURNOUT;
			print_log(sim->hub[i], now, false);
			pthread_mutex_unlock(&sim->hub[i]->lock);
			return (SHUTDOWN_SIGNAL);
		}
		if (sim->hub[i]->status != ALL_COMPILES_COMPLETED)
			status = COMPILING;
		pthread_mutex_unlock(&sim->hub[i]->lock);
		update_cooldown(sim->quantum[i], &sim->start);
	}
	return (status);
}

void	*monitor_routine(void *arg)
{
	t_monitor			*monitor;
	t_status			status;
	int					i;

	monitor = (t_monitor *)arg;
	status = AVAILABLE;
	while (status != SHUTDOWN_SIGNAL)
	{
		status = check_coder_status_and_cooldown(monitor->sim);
		if (status == SHUTDOWN_SIGNAL)
			sim_lock_and_access(monitor->sim, status, true);
		else
			usleep(MONITOR_SLEEP);
	}
	i = -1;
	while (++i < monitor->sim->number_of_coders)
	{
		pthread_mutex_lock(&monitor->sim->quantum[i]->lock);
		pthread_cond_broadcast(&monitor->sim->quantum[i]->cond);
		pthread_mutex_unlock(&monitor->sim->quantum[i]->lock);
	}
	return (NULL);
}
