/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 17:16:45 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/26 11:20:56 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_coders(t_coder **hub, int i)
{
	while (--i >= 0)
	{
		if (hub[i]->has_thread)
			pthread_join(hub[i]->thread, NULL);
		if (hub[i]->has_lock)
			pthread_mutex_destroy(&hub[i]->lock);
		free(hub[i]);
	}
	free(hub);
}

void	free_quantum(t_dongle **quantum, int i)
{
	while (--i >= 0)
	{
		if (quantum[i]->has_lock)
			pthread_mutex_destroy(&quantum[i]->lock);
		if (quantum[i]->has_cond)
			pthread_cond_destroy(&quantum[i]->cond);
		free(quantum[i]);
	}
	free(quantum);
}

void	free_hub_memory(t_simulation *sim)
{
	if (sim->monitor.has_thread)
		pthread_join(sim->monitor.thread, NULL);
	free_coders(sim->hub, sim->n_coders);
	free_quantum(sim->quantum, sim->n_dongles);
	if (sim->has_lock)
		pthread_mutex_destroy(&sim->lock);
	if (sim->has_log)
		pthread_mutex_destroy(&sim->log);
	sim->status = SUCCESS;
}
