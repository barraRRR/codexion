/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 10:26:13 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/22 11:36:28 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    update_cooldown(t_dongle *usb, struct timeval *start)
{
    long long           now;
    long long           target;

    target = usb->last_compile_time + usb->sim->dongle_cooldown;
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