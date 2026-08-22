/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 10:29:43 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/22 10:30:48 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void compile_init(t_coder *coder)
{
    coder->status = COMPILING;
    print_log(coder, timer(&coder->sim->start));
    lock_dongles_in_order(coder);
    vigilant_sleep(coder, coder->sim->time_to_compile * 1000);
    coder->usb_right->last_compile_time = timer(&coder->sim->start);
    coder->usb_left->last_compile_time = timer(&coder->sim->start);
    coder->usb_right->status = COOLING_DOWN;
    coder->usb_left->status = COOLING_DOWN;
    pthread_mutex_lock(&coder->sim->lock_compiles);
    (coder->sim->completed_compiles)++;
    pthread_mutex_unlock(&coder->sim->lock_compiles);
    unlock_dongles(coder);
    coder->status = COMPILING_COMPLETED;
}

void debug_init(t_coder *coder)
{
    coder->status = DEBUGGING;
    print_log(coder, timer(&coder->sim->start));
    usleep(coder->sim->time_to_debug * 1000);
    coder->status = DEBUGGING_COMPLETED;
}

void refactor_init(t_coder *coder)
{
    coder->status = REFACTORING;
    print_log(coder, timer(&coder->sim->start));
    usleep(coder->sim->time_to_refactor * 1000);
    coder->status = WAITING_DONGLE;
    pthread_mutex_lock(&coder->sim->lock_compiles);
    (coder->sim->completed_compiles)++;
    pthread_mutex_unlock(&coder->sim->lock_compiles);
}
