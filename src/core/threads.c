/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 13:09:10 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/21 19:52:38 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int    init_threads(t_simulation *sim)
{
    int             i;

    pthread_mutex_init(&sim->lock_compiles, NULL);    // necesita protección?
    pthread_mutex_init(&sim->lock_log, NULL);    // necesita protección?
    pthread_mutex_init(&sim->lock_status, NULL);    // necesita protección?
    i = 0;
    while (i < sim->number_of_coders)
    {
        if (pthread_mutex_init(&sim->quantum[i]->lock, NULL))
            return (false);
        if (pthread_cond_init(&sim->quantum[i]->cond, NULL))
            return (false);
        i++;
    }
    i = 0;
    while (i < sim->number_of_coders)
    {
        if (pthread_create(&sim->hub[i]->thread, NULL, quantum_compiler, sim->hub[i]))
            return (false);
        i++;
    }
    if (pthread_create(&sim->monitor.thread, NULL, monitor_routine, &sim->monitor))
        return (false);
    return (true);
}

void    join_threads(t_coder **hub, int number_of_coders)
{
    int             i;

    i = 0;
    while (i < number_of_coders)
    {
        pthread_join(hub[i]->thread, NULL);
        i++;
    }
}