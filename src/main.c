/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/22 11:31:46 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    print_hub(t_coder **hub, int number_of_coders)
{
    int             i;

    i = 0;
    while (i < number_of_coders)
    {
        printf("CODER %d\n", hub[i]->id);
        printf("  *right usb: %d\n", hub[i]->usb_right->id);
        printf("  *left usb : %d\n", hub[i]->usb_left->id);
        i++;
    }
}

bool    safe_status(t_coder *coder, t_status status)
{
    pthread_mutex_lock(&coder->lock_status);
    if (coder->status == status)
    {
        pthread_mutex_unlock(&coder->lock_status);
        return (true);
    }
    pthread_mutex_unlock(&coder->lock_status);
    return (false);
}

void    *quantum_compiler(void *arg)
{
    t_coder                 *coder;
    
    coder = (t_coder *)arg;
    while (true)
    {
        if (check_completion(coder) || safe_status(coder, BURNOUT))
            return (NULL);
        else if (safe_status(coder, CODER_INIT) || safe_status(coder, REFACTORING_COMPLETED))
            enqueue_coder(coder);
        else if (safe_status(coder, WAITING_DONGLE))
            take_dongle(coder);
        else if (safe_status(coder, TAKING_DONGLE))
            compile_init(coder);
        else if (safe_status(coder, COMPILING_COMPLETED))
            debug_init(coder);
        else if (safe_status(coder, DEBUGGING_COMPLETED))
            refactor_init(coder);
    }
    return (NULL);
}

void    destroy_mutex_and_cond(t_simulation *sim)
{
    int                 i;

    i = 0;
    while (i < sim->number_of_coders)
    {
        pthread_mutex_destroy(&sim->hub[i]->lock_status);
        pthread_mutex_destroy(&sim->quantum[i]->lock);
        pthread_cond_destroy(&sim->quantum[i]->cond);
        i++;
    }
    pthread_mutex_destroy(&sim->lock_compiles);
    pthread_mutex_destroy(&sim->lock_log);
    pthread_mutex_destroy(&sim->lock_status);
}

int main(int argc, char **argv)
{
    t_simulation            sim;

    gettimeofday(&sim.start, NULL);
    sim.monitor.sim = &sim;
    sim.status = parse_rules(&sim, argc, argv);
    if (sim.status != PARSING_COMPLETED)
        return (sim.status);
    print_status(sim);
    sim.status = init_coworking(&sim);
    if (sim.status == MALLOC_ERR)
        return (print_err(MALLOC_ERR, MALLOC_ERR_MSG));
    print_hub(sim.hub, sim.number_of_coders);
    if (!init_threads(&sim))
        return (print_err(INIT_THREAD_FAILED, THREAD_ERR_MSG));      // aquí no estoy liberando la memoria
    destroy_mutex_and_cond(&sim);       // Necesita protecciones?
    join_threads(sim.hub, sim.number_of_coders);
    free_hub_memory(&sim, sim.number_of_coders);
    return (SUCCESS);
}
