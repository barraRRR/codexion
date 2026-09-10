/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/01 08:02:15 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_simulation		sim;

	sim.monitor.sim = &sim;
	sim.has_lock = false;
	sim.has_log = false;
	sim.hub = NULL;
	sim.quantum = NULL;
	sim.n_coders = 0;
	sim.n_dongles = 0;
	sim.status = parse_rules(&sim, argc, argv);
	if (sim.status != PARSING_COMPLETED)
		return (sim.status);
	sim.status = init_coworking(&sim);
	if (sim.status == MALLOC_ERR)
		return (print_err(MALLOC_ERR, MALLOC_ERR_MSG, &sim, true));
	gettimeofday(&sim.start, NULL);
	if (init_threads(&sim) != INIT_SIMULATION)
 		return (print_err(INIT_THREADS_ERR, THREAD_ERR_MSG, &sim, true));
	free_hub_memory(&sim);
	return (sim.status);
}
