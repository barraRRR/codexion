/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_memory.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 17:16:45 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/29 12:41:44 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_status	free_all_memory(t_simulation *sim, t_status status)
{
	int					i;

	pthread_destroy_monitor(&sim->monitor);
	i = -1;
	while (++i < sim->n_coders_init && sim->hub && sim->hub[i])
		pthread_destroy_coder(sim->hub[i]);
	i = -1;
	while (++i < sim->n_dongles_init && sim->quantum && sim->quantum[i])
		pthread_destroy_dongle(sim->quantum[i]);
	pthread_destroy_printer(&sim->printer);
	pthread_destroy_sim(sim);
	i = -1;
	while (++i < sim->n_coders_init && sim->hub && sim->hub[i])
		free(sim->hub[i]);
	i = -1;
	while (++i < sim->n_dongles_init && sim->quantum && sim->quantum[i])
		free(sim->quantum[i]);
	if (sim->hub)
		free(sim->hub);
	if (sim->quantum)
		free(sim->quantum);
	free_logs(&sim->printer);
	return (status);
}
