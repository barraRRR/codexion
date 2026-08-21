/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/21 13:38:56 by jbarreir         ###   ########.fr       */
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

void    update_cooldown(t_dongle *usb, struct timeval *start)
{
    long long           now;
    long long           target;

    target = usb->last_compile_time + usb->dongle_cooldown;
    now = timer(start);
    if (target <= now)
    {
        usb->status = AVAILABLE;
        pthread_cond_broadcast(&usb->cond);
    }
        else usb->status = COOLING_DOWN;
}

bool    usb_access(t_dongle *usb, t_coder *coder, struct timeval *start)
{
    if (usb->status == PLUGGED)
        return (false);
    update_cooldown(usb, start);
    if (usb->status == COOLING_DOWN)
        return (false);
    if (usb->queue[0] && usb->queue[0] != coder)
        return (false); 
    return (true);
}

bool    both_usb_access(t_coder *coder)
{
    bool                validator_right;
    bool                validator_left;

    validator_right = usb_access(coder->usb_right, coder, &coder->sim->start);
    validator_left = usb_access(coder->usb_left, coder, &coder->sim->start);
    if (validator_right && validator_left)
        return (true);
    return (false);
}   

void    lock_dongles_in_order(t_coder *coder)
{
    if (coder->usb_right->id <= coder->usb_left->id)
    {
        pthread_mutex_lock(&coder->usb_right->lock);
        pthread_mutex_lock(&coder->usb_left->lock);
    }
    else
    {
        pthread_mutex_lock(&coder->usb_left->lock);
        pthread_mutex_lock(&coder->usb_right->lock);
    }
}

void    unlock_dongles(t_coder *coder)
{
    pthread_mutex_unlock(&coder->usb_right->lock);
    pthread_mutex_unlock(&coder->usb_left->lock);
}

void    take_dongle(t_coder *coder)
{
    lock_dongles_in_order(coder);
    while (!both_usb_access(coder))
    {
        pthread_mutex_unlock(&coder->usb_left->lock);
        pthread_cond_wait(&coder->usb_right->cond, &coder->usb_right->lock);
        pthread_mutex_lock(&coder->usb_left->lock);
    }
    coder->status = TAKING_DONGLE;
    print_log(coder, timer(&coder->sim->start));
    coder->usb_right->status = PLUGGED;
    print_log(coder, timer(&coder->sim->start));
    coder->usb_left->status = PLUGGED;
    dequeue(coder->usb_right->queue);
    dequeue(coder->usb_left->queue);
    unlock_dongles(coder);
}

void    compile_init(t_coder *coder)
{
    coder->status = COMPILING;
    print_log(coder, timer(&coder->sim->start));
    usleep(coder->sim->time_to_compile * 1000);
    lock_dongles_in_order(coder);
    coder->usb_right->last_compile_time = timer(&coder->sim->start);
    coder->usb_left->last_compile_time = timer(&coder->sim->start);
    coder->usb_right->status = COOLING_DOWN;
    coder->usb_left->status = COOLING_DOWN;
    unlock_dongles(coder);
    coder->status = COMPILING_COMPLETED;
}

void    debug_init(t_coder *coder)
{
    coder->status = DEBUGGING;
    print_log(coder, timer(&coder->sim->start));
    usleep(coder->sim->time_to_debug * 1000);
    coder->status = DEBUGGING_COMPLETED;
}

void    refactor_init(t_coder *coder)
{
    coder->status = REFACTORING;
    print_log(coder, timer(&coder->sim->start));
    usleep(coder->sim->time_to_refactor * 1000);
    coder->status = WAITING_DONGLE;
    pthread_mutex_lock(&coder->sim->lock_compiles);
    (coder->sim->completed_compiles)++;
    pthread_mutex_unlock(&coder->sim->lock_compiles);
}

void    *quantum_compiler(void *arg)
{
    t_coder                 *coder;
    
    coder = (t_coder *)arg;
    while (true)
    {
        if (coder->status == CODER_INIT || coder->status == REFACTORING_COMPLETED)
            enqueue_coder(coder);
        else if (coder->status == WAITING_DONGLE)
            take_dongle(coder);
        else if (coder->status == TAKING_DONGLE)
            compile_init(coder);
        else if (coder->status == COMPILING_COMPLETED)
            debug_init(coder);
        else if (coder->status == DEBUGGING_COMPLETED)
            refactor_init(coder);
    }
    return (coder);
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
        return (print_err(INIT_THREAD_FAILED, THREAD_ERR_MSG));         // aquí no estoy liberando la memoria
    join_threads(sim.hub, sim.number_of_coders);
    free_hub_memory(&sim, sim.number_of_coders);
    return (SUCCESS);
}
