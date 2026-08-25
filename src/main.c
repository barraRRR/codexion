/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/25 17:41:03 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
TODO:
- Liberar memoria correctamente en caso de fallo
- Destruir solo los mutex e hilos que son creados
*/
int	main(int argc, char **argv)
{
	t_simulation		sim;

	gettimeofday(&sim.start, NULL);
	sim.monitor.sim = &sim;
	sim.status = parse_rules(&sim, argc, argv);
	if (sim.status != PARSING_COMPLETED)
		return (sim.status);
	sim.status = init_coworking(&sim);
	if (sim.status == MALLOC_ERR)
	{
		free_hub_memory(&sim);
		return (print_err(MALLOC_ERR, MALLOC_ERR_MSG));
	}
	if (init_threads(&sim) != INIT_SIMULATION)
	{
		free_hub_memory(&sim);
		return (print_err(INIT_THREADS_ERR, THREAD_ERR_MSG));
	}
	free_hub_memory(&sim);
	return (sim.status);
}
