/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_memory.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 17:16:45 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/28 19:58:20 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_status	free_all_memory(t_simulation *sim, t_status status, int i)
{
	while (--i >= 0)
	{
		if (sim->hub && sim->hub[i])
		{
			pthread_destroy_coder(sim->hub[i]);
			free(sim->hub[i]);
		}
		if (sim->quantum && sim->quantum[i])
		{
			pthread_destroy_dongle(sim->quantum[i]);
			free(sim->quantum[i]);
		}
	}
	if (sim->hub)
		free(sim->hub);
	if (sim->quantum)
		free(sim->quantum);
	free_logs(&sim->printer, true);
	pthread_destroy_monitor(&sim->monitor);
	pthread_destroy_printer(&sim->printer);
	return (status);
}
