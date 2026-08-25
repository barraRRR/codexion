/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:52:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/25 10:16:01 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void update_cooldown(t_dongle *usb, struct timeval *start)
{
    long long now;
    long long target;

    pthread_mutex_lock(&usb->lock);
    if (usb->status == COOLING_DOWN)
    {
        target = usb->last_compile_time + usb->sim->dongle_cooldown;
        now = timer(start);
        if (now >= target)
        {
            usb->status = AVAILABLE;
            pthread_cond_broadcast(&usb->cond);
        }
    }
    pthread_mutex_unlock(&usb->lock);
}

bool    check_for_burnout(t_coder *coder)
{
    long long                 now;
    long long                 last_compile;
    bool                        result;

    result = false;
    pthread_mutex_lock(&coder->lock);
    last_compile = coder->last_compile_time;
    now = timer(&coder->sim->start);
    if (now >= last_compile + coder->sim->time_to_burnout)
    {
        coder->status = BURNOUT;
        print_log(coder, now, false);
        sim_lock_and_access(coder->sim, BURNOUT, true);
        result = true;
    }
    pthread_mutex_unlock(&coder->lock);
    return (result);
}

bool    check_compiles(t_simulation *sim)
{
    bool                completed;
    
    pthread_mutex_lock(&sim->lock);
    completed = sim->completed_compiles >= sim->compiles_required;
    pthread_mutex_unlock(&sim->lock);
    return (completed);
}

void    *monitor_routine(void *arg)
{
    t_monitor           *monitor;
    bool                running;
    int                 i;
    
    monitor = (t_monitor*)arg;
    running = true;
    while(running)
    {
        if (sim_lock_and_access(monitor->sim, BURNOUT, false))
            running = false;
        else if (sim_lock_and_access(monitor->sim, COMPILING_COMPLETED, false))
            running = false;
        else if (check_compiles(monitor->sim))
        {
            sim_lock_and_access(monitor->sim, COMPILING_COMPLETED, true);
            running = false;
        }
        else
            running = true;
        i = -1;
        while (++i < monitor->sim->number_of_coders && running)
        {
            update_cooldown(monitor->sim->quantum[i], &monitor->sim->start);
            if (check_for_burnout(monitor->sim->hub[i]))
            {
                sim_lock_and_access(monitor->sim, BURNOUT, true);
                running = false;
            }
        }
        if (running)
            usleep(MONITOR_SLEEP);
    }
    i = -1;
    while (++i < monitor->sim->number_of_coders)
    {
        pthread_mutex_lock(&monitor->sim->quantum[i]->lock);
        pthread_cond_broadcast(&monitor->sim->quantum[i]->cond);
        pthread_mutex_unlock(&monitor->sim->quantum[i]->lock);
    }
    return (NULL);
}