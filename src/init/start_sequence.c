/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_sequence.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 16:10:23 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/23 16:58:53 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
