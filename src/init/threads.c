/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 13:09:10 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/25 17:50:57 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_status	init_threads(t_simulation *sim)
{
	int					i;

	if (pthread_mutex_init(&sim->log, NULL))
		return (INIT_LOG_ERR);
	if (pthread_mutex_init(&sim->lock, NULL))
		return (INIT_SIM_LOCK_ERR);
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
