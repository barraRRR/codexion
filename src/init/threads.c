/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 13:09:10 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/23 17:52:42 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_status	init_threads(t_simulation *sim)
{
	int					i;

	if (pthread_mutex_init(&sim->log, NULL))
		return (INIT_LOG_ERR);
	sim->has_log = true;
	if (pthread_mutex_init(&sim->lock, NULL))
		return (INIT_SIM_LOCK_ERR);
	sim->has_lock = true;
	i = -1;
	while (++i < sim->number_of_coders)
	{
		if (pthread_create(&sim->hub[i]->thread, NULL, quantum_compiler,
				sim->hub[i]))
			return (INIT_THREADS_ERR);
		sim->hub[i]->has_thread = true;
	}
	sim->monitor.has_thread = false;
	if (pthread_create(&sim->monitor.thread, NULL, monitor_routine,
			&sim->monitor))
		return (INIT_MONITOR_ERR);
	sim->monitor.has_thread = true;
	start_sequence(sim);
	return (INIT_SIMULATION);
}

void	join_threads_and_destroy_mutex_cond(t_simulation *sim)
{
	int					i;

	if (!sim)
		return ;
	i = -1;
	while (++i < sim->number_of_coders)
	{
		if (sim->hub && sim->hub[i])
		{
			pthread_join(sim->hub[i]->thread, NULL);
			pthread_mutex_destroy(&sim->hub[i]->lock);
		}
	}
	pthread_join(sim->monitor.thread, NULL);
	i = -1;
	while (++i < sim->number_of_coders)
	{
		if (sim->quantum && sim->quantum[i])
		{
			pthread_mutex_destroy(&sim->quantum[i]->lock);
			pthread_cond_destroy(&sim->quantum[i]->cond);
		}
	}
	pthread_mutex_destroy(&sim->lock);
	pthread_mutex_destroy(&sim->log);
}

t_status	init_start_cond(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->start_lock, NULL))
		return (INIT_START_COND_ERR);
	if (pthread_cond_init(&sim->start_cond, NULL))
	{
		pthread_mutex_destroy(&sim->start_lock);
		return (INIT_START_COND_ERR);
	}
	return (INIT_START_COND);
}

void	wait_for_start_sequence(t_simulation *sim)
{
	pthread_mutex_lock(&sim->start_lock);
	while (!sim->is_started)
		pthread_cond_wait(&sim->start_cond, &sim->start_lock);
	pthread_mutex_unlock(&sim->start_lock);
}

void	start_sequence(t_simulation *sim)
{
	gettimeofday(&sim->start, NULL);
	pthread_mutex_lock(&sim->start_lock);
	sim->is_started = true;
	pthread_cond_broadcast(&sim->start_cond);
	pthread_mutex_unlock(&sim->start_lock);
}
