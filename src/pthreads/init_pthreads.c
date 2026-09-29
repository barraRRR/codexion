/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_pthreads.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 13:09:10 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/29 12:56:28 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_status	init_sim_pthread(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->lock, NULL))
		return (INIT_SIM_LOCK_ERR);
	sim->has_lock = true;
	if (pthread_mutex_init(&sim->start_lock, NULL))
		return (INIT_START_COND_ERR);
	sim->has_start_lock = true;
	if (pthread_cond_init(&sim->start_cond, NULL))
		return (INIT_START_COND_ERR);
	sim->has_start_cond = true;
	if (pthread_mutex_init(&sim->printer.lock, NULL))
		return (INIT_PRINTER_ERR);
	sim->printer.has_lock = true;
	if (pthread_cond_init(&sim->printer.cond, NULL))
		return (INIT_PRINTER_ERR);
	sim->printer.has_cond = true;
	return (INIT_SIMULATION);
}

t_status	init_threads(t_simulation *sim)
{
	int					i;

	sim->status = init_sim_pthread(sim);
	if (sim->status != INIT_SIMULATION)
		return (sim->status);
	i = -1;
	while (++i < sim->n_coders)
	{
		if (pthread_create(&sim->hub[i]->thread, NULL, quantum_compiler,
						   sim->hub[i]))
			return (abort_start_sequence(sim, INIT_THREADS_ERR));
		sim->hub[i]->has_thread = true;
	}
	if (pthread_create(&sim->monitor.thread, NULL, monitor_routine,
			&sim->monitor))
		return (abort_start_sequence(sim, INIT_MONITOR_ERR));
	sim->monitor.has_thread = true;
	if (pthread_create(&sim->printer.thread, NULL, printer_routine,
			&sim->printer))
		return (abort_start_sequence(sim, INIT_PRINTER_ERR));
	sim->printer.has_thread = true;
	start_sequence(sim);
	return (INIT_SIMULATION);
}
