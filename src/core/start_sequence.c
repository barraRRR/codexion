/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_sequence.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 09:58:27 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/28 18:30:06 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wait_for_start_sequence(t_simulation *sim)
{
	pthread_mutex_lock(&sim->start_lock);
	while (!sim->is_started)
		pthread_cond_wait(&sim->start_cond, &sim->start_lock);
	pthread_mutex_unlock(&sim->start_lock);
}

void	start_sequence(t_simulation *sim)
{
	gettimeofday(&sim->start_time, NULL);
	pthread_mutex_lock(&sim->start_lock);
	sim->is_started = true;
	pthread_cond_broadcast(&sim->start_cond);
	pthread_mutex_unlock(&sim->start_lock);
}

t_status	abort_start_sequence(t_simulation *sim, t_status status)
{
	sim_lock_and_access(sim, SHUTDOWN, true);
	pthread_mutex_lock(&sim->start_lock);
	sim->is_started = true;
	pthread_cond_broadcast(&sim->start_cond);
	pthread_mutex_unlock(&sim->start_lock);
	return (status);
}
