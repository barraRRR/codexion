/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_memory.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 17:16:45 by jbarreir          #+#    #+#             */
/*   Updated: 2026/10/01 11:02:54 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	free_heap(t_simulation *sim)
{
	int					i;

	i = -1;
	while (++i < sim->n_coders_init && sim->hub && sim->hub[i])
		free(sim->hub[i]);
	i = -1;
	while (++i < sim->n_dongles_init && sim->quantum && sim->quantum[i])
		free(sim->quantum[i]);
	i = -1;
	while (++i < sim->monitor.n_sub && sim->monitor.pool
		&& sim->monitor.pool[i])
		free(sim->monitor.pool[i]);
	if (sim->hub)
		free(sim->hub);
	sim->hub = NULL;
	if (sim->quantum)
		free(sim->quantum);
	sim->quantum = NULL;
	if (sim->monitor.pool)
		free(sim->monitor.pool);
	sim->monitor.pool = NULL;
	free_logs(&sim->printer);
}

t_status	free_all_memory(t_simulation *sim, t_status status)
{
	pthread_destroy_monitor(&sim->monitor);
	pthread_destroy_hub(sim);
	pthread_destroy_dongle(sim);
	pthread_destroy_printer(&sim->printer);
	pthread_destroy_sim(sim);
	free_heap(sim);
	return (status);
}
