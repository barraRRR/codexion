/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_pthreads.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 13:09:10 by jbarreir          #+#    #+#             */
/*   Updated: 2026/10/01 10:57:49 by jbarreir         ###   ########.fr       */
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

static bool	init_quantum_compiler(t_simulation *sim)
{
	int					i;

	i = -1;
	while (++i < sim->n_coders)
	{
		if (pthread_create(&sim->hub[i]->thread, NULL, quantum_compiler,
				sim->hub[i]))
			return (false);
		sim->hub[i]->has_thread = true;
	}
	return (true);
}

static bool	init_pool_routine(t_simulation *sim)
{
	int					i;

	i = -1;
	while (++i < sim->monitor.n_sub)
	{
		if (pthread_create(&sim->monitor.pool[i]->thread, NULL,
				submonitor_routine, sim->monitor.pool[i]))
			return (false);
		sim->monitor.pool[i]->has_thread = true;
	}
	return (true);
}

t_status	init_threads(t_simulation *sim)
{
	sim->status = init_sim_pthread(sim);
	if (sim->status != INIT_SIMULATION)
		return (sim->status);
	if (!init_quantum_compiler(sim))
		return (abort_start_sequence(sim, INIT_THREADS_ERR));
	if (pthread_create(&sim->monitor.thread, NULL, monitor_routine,
			&sim->monitor))
		return (abort_start_sequence(sim, INIT_MONITOR_ERR));
	sim->monitor.has_thread = true;
	if (!init_pool_routine(sim))
		return (abort_start_sequence(sim, INIT_MONITOR_ERR));
	if (pthread_create(&sim->printer.thread, NULL, printer_routine,
			&sim->printer))
		return (abort_start_sequence(sim, INIT_PRINTER_ERR));
	sim->printer.has_thread = true;
	start_sequence(sim);
	return (INIT_SIMULATION);
}
