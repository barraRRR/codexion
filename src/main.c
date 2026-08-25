/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/25 13:12:35 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int main(int argc, char **argv)
{
    t_simulation            sim;

    gettimeofday(&sim.start, NULL);
    sim.monitor.sim = &sim;
    sim.status = parse_rules(&sim, argc, argv);
    if (sim.status != PARSING_COMPLETED)
        return (sim.status);
    sim.status = init_coworking(&sim);
    if (sim.status == MALLOC_ERR)
        return (print_err(MALLOC_ERR, MALLOC_ERR_MSG));
    if (init_threads(&sim) != INIT_SIMULATION)
    {
        free_hub_memory(&sim, sim.number_of_coders);
        return (print_err(INIT_THREADS_ERR, THREAD_ERR_MSG));      // aquí no estoy liberando la memoria
    }
    join_threads_and_destroy_mutex_cond(&sim);      // tengo que asegurarme de liberar mutex y cond si hay fallos
    free_hub_memory(&sim, sim.number_of_coders);
    return (sim.status);
}
