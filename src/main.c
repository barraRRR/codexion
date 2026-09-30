/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/30 09:53:14 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_simulation		sim;

	init_sim_data(&sim);
	sim.status = parse_rules(&sim, argc, argv);
	if (sim.status != PARSING_COMPLETED)
		return (sim.status);
	if (sim.compiles_required == 0)
		return (SUCCESS);
	sim.status = init_coworking(&sim);
	if (sim.status == MALLOC_ERR)
		return (print_err(MALLOC_ERR, MALLOC_ERR_MSG, &sim, true));
	if (init_threads(&sim) != INIT_SIMULATION)
		return (print_err(INIT_THREADS_ERR, THREAD_ERR_MSG, &sim, true));
	return (free_all_memory(&sim, SUCCESS));
}
