/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 10:29:43 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/25 14:05:04 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void take_dongle(t_coder *coder)
{
    t_status            available;

    lock_dongles_in_order(coder);
    available = both_usb_access(coder);
    while (available != AVAILABLE_BOTH)
    {
        pthread_mutex_unlock(&coder->lock);
        if (available == AVAILABLE_LEFT || available == AVAILABLE_NONE)
        {
            pthread_mutex_unlock(&coder->usb_left->lock);
            pthread_cond_wait(&coder->usb_right->cond, &coder->usb_right->lock);
            pthread_mutex_lock(&coder->usb_left->lock);
        }
        else if (available == AVAILABLE_RIGHT)
        {
            pthread_mutex_unlock(&coder->usb_right->lock);
            pthread_cond_wait(&coder->usb_left->cond, &coder->usb_left->lock);
            pthread_mutex_lock(&coder->usb_right->lock);
        }
        pthread_mutex_lock(&coder->lock);
        available = both_usb_access(coder);
        if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
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
    coder->completed_compiles++;
    if (coder->completed_compiles >= coder->sim->compiles_required)
        coder->status = ALL_COMPILES_COMPLETED;
    else
        coder->status = COMPILING_COMPLETED;
    pthread_mutex_unlock(&coder->lock);
    coder->usb_right->last_compile_time = time;
    coder->usb_left->last_compile_time = time;
    coder->usb_right->status = COOLING_DOWN;
    coder->usb_left->status = COOLING_DOWN;
    unlock_dongles(coder);
}

void    debug_and_refactor(t_coder *coder)
{
    coder->status = DEBUGGING;
    print_log(coder, timer(&coder->sim->start), false);
    pthread_mutex_unlock(&coder->lock);
    vigilant_sleep(coder, coder->sim->time_to_debug);
    if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
        return ;
    pthread_mutex_lock(&coder->lock);
    coder->status = REFACTORING;
    print_log(coder, timer(&coder->sim->start), false);
    pthread_mutex_unlock(&coder->lock);
    vigilant_sleep(coder, coder->sim->time_to_refactor);
    if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
        return ;
    coder_lock_and_access(coder, REFACTORING_COMPLETED, true, true);
}

void *quantum_compiler(void *arg)
{
    t_coder         *coder;
    t_status        status;

    coder = (t_coder *)arg;
    while (true)
    {
        if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
            break ;
        pthread_mutex_lock(&coder->lock);
        status = coder->status;
        if (status == BURNOUT || status == ALL_COMPILES_COMPLETED)
        {
            pthread_mutex_unlock(&coder->lock);
            break ;
        }
        if (status == CODER_INIT || status == REFACTORING_COMPLETED)
            enqueue_coder(coder);
        else if (status == WAITING_DONGLE)
            take_dongle(coder);
        else if (status == TAKING_DONGLE)
            compile_init(coder);
        else if (status == COMPILING_COMPLETED)
            debug_and_refactor(coder);
    }
    return (NULL);
}