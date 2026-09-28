/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:52:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/28 19:54:20 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_status check_coder(t_coder *coder)
{
	t_status			status;

	status = SHUTDOWN;
	pthread_mutex_lock(&coder->lock);
	if (coder->status != ALL_COMPILES_COMPLETED && (coder->status == BURNOUT
		|| am_i_burnt(coder)))
	{
		if (!append_log(coder, timer(coder->sim)))
			return (exit_code_unlock(coder, MALLOC_ERR, false));
		status = SHUTDOWN;
	}
	else if (coder->status != ALL_COMPILES_COMPLETED)
		status = COMPILING;
	pthread_mutex_unlock(&coder->lock);
	return (status);
}

static t_status check_hub(t_simulation *sim)
{
	int					i;

	i = -1;
	while (++i < sim->n_coders)
	{
		update_cooldown(sim->quantum[i], sim);
		if (check_coder(sim->hub[i]) == SHUTDOWN)
			return (SHUTDOWN);
	}
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
