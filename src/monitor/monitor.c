/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:52:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/29 12:57:07 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_status check_coder(t_coder *coder)
{
	t_status			status;

	status = COMPILING;
	pthread_mutex_lock(&coder->lock);
	if (coder->status == ALL_COMPILES_COMPLETED)
		status = ALL_COMPILES_COMPLETED;
	else if (coder->status == BURNOUT || am_i_burnt(coder))
	{
		status = BURNOUT;
		if (!append_log(coder, timer(coder->sim)))
			status = MALLOC_ERR;
	}
	pthread_mutex_unlock(&coder->lock);
	return (status);
}

static t_status check_hub(t_simulation *sim)
{
	int					i;
	int					active;
	t_status			status;

	i = -1;
	active = 0;
	while (++i < sim->n_coders)
	{
		update_cooldown(sim->quantum[i], sim);
		status = check_coder(sim->hub[i]);
		if (status == BURNOUT || status == MALLOC_ERR)
		{
			sim->printer.stop = true;
			return (SHUTDOWN);
		}
		if (status == COMPILING)
			active++;
	}
	if (active == 0)
		return (SHUTDOWN);
	return (COMPILING);
}

static void	wake_dongles(t_monitor *monitor)
{
	int					i;

	i = -1;
	while (++i < monitor->sim->n_coders)
	{
		pthread_mutex_lock(&monitor->sim->quantum[i]->lock);
		pthread_cond_broadcast(&monitor->sim->quantum[i]->cond);
		pthread_mutex_unlock(&monitor->sim->quantum[i]->lock);
	}
}

void	*monitor_routine(void *arg)
{
	t_monitor			*monitor;
	t_status			status;

	monitor = (t_monitor *)arg;
	wait_for_start_sequence(monitor->sim);
	status = AVAILABLE;
	while (status != SHUTDOWN)
	{
		status = check_hub(monitor->sim);
		if (status == SHUTDOWN)
			sim_lock_and_access(monitor->sim, status, true);
		else
			usleep(MONITOR_SLEEP);
	}
	wake_dongles(monitor);
	return (NULL);
}
