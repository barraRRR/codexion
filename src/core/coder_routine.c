/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 10:29:43 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/24 18:28:28 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void take_dongle(t_coder *coder)
{
    lock_dongles_in_order(coder);
    while (!both_usb_access(coder))
    {
        pthread_mutex_unlock(&coder->lock);
        pthread_mutex_unlock(&coder->usb_left->lock);
        pthread_cond_wait(&coder->usb_right->cond, &coder->usb_right->lock);
        pthread_mutex_lock(&coder->usb_left->lock);
        pthread_mutex_lock(&coder->lock);
        if (check_completion(coder) || is_burnout(coder))
        {
            unlock_dongles(coder);
            pthread_mutex_unlock(&coder->lock);
            return ;
        }
    }
    coder->status = TAKING_DONGLE;
    print_log(coder, timer(&coder->sim->start), false);
    coder->usb_right->status = PLUGGED;
    print_log(coder, timer(&coder->sim->start), false);
    coder->usb_left->status = PLUGGED;
    dequeue(coder->usb_right->queue);
    dequeue(coder->usb_left->queue);
    unlock_dongles(coder);
    pthread_mutex_unlock(&coder->lock);
}

void compile_init(t_coder *coder)
{
    long long               time;

    coder->status = COMPILING;
    print_log(coder, timer(&coder->sim->start), false);
    pthread_mutex_unlock(&coder->lock);
    vigilant_sleep(coder, coder->sim->time_to_compile);
    if (is_burnout(coder))
        return ;
    lock_dongles_in_order(coder);
    time = timer(&coder->sim->start);
    pthread_mutex_lock(&coder->lock);
    coder->last_compile_time = time;
    pthread_mutex_unlock(&coder->lock);
    coder->usb_right->last_compile_time = time;
    coder->usb_left->last_compile_time = time;
    coder->usb_right->status = COOLING_DOWN;
    coder->usb_left->status = COOLING_DOWN;
    unlock_dongles(coder);
    add_simulation(coder->sim);
    coder_lock_and_access(coder, COMPILING_COMPLETED, true, true);
}

void debug_init(t_coder *coder)
{
    coder->status = DEBUGGING;
    print_log(coder, timer(&coder->sim->start), false);
    pthread_mutex_unlock(&coder->lock);
    vigilant_sleep(coder, coder->sim->time_to_debug);
    if (is_burnout(coder))
        return;
    coder_lock_and_access(coder, DEBUGGING_COMPLETED, true, true);
}

void refactor_init(t_coder *coder)
{
    coder->status = REFACTORING;
    print_log(coder, timer(&coder->sim->start), false);
    pthread_mutex_unlock(&coder->lock);
    vigilant_sleep(coder, coder->sim->time_to_refactor);
    if (is_burnout(coder))
        return;
    coder_lock_and_access(coder, REFACTORING_COMPLETED, true, true);
}

void *quantum_compiler(void *arg)
{
    t_coder         *coder;
    bool            running;
    t_status        status;

    coder = (t_coder *)arg;
    running = true;
    while (running)
    {
        pthread_mutex_lock(&coder->sim->lock);
        status = coder->sim->status;
        pthread_mutex_unlock(&coder->sim->lock);
        if (status == BURNOUT || status == COMPILING_COMPLETED)
        {
            break ;
        }
        pthread_mutex_lock(&coder->lock);
        status = coder->status;
        if (status == CODER_INIT || status == REFACTORING_COMPLETED)
        {
            enqueue_coder(coder);
        }
        else if (status == WAITING_DONGLE)
        {
            take_dongle(coder);
        }
        else if (status == TAKING_DONGLE)
        {
            compile_init(coder);
        }
        else if (status == COMPILING_COMPLETED)
            debug_init(coder);
        else if (status == DEBUGGING_COMPLETED)
            refactor_init(coder);
    }
    return (NULL);
}