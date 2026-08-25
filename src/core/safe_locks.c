/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_locks.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 14:26:38 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/25 13:44:28 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool    sim_lock_and_access(t_simulation *sim, t_status status, bool update)
{
    bool                result;

    pthread_mutex_lock(&sim->lock);
    if (sim->status == status)
        result = true;
    else
        result = false;
    if (update && sim->status != SHUTDOWN_SIGNAL)
        sim->status = status;
    pthread_mutex_unlock(&sim->lock);
    return (result);
}
    
bool    coder_lock_and_access(t_coder *coder, t_status status, bool update, bool unlock)
{
    bool                result;
    
    pthread_mutex_lock(&coder->lock);
    if (coder->status == status)
        result = true;
    else
        result = false;
    if (update && coder->status != BURNOUT)
        coder->status = status;
    if (unlock)
        pthread_mutex_unlock(&coder->lock);
    return (result);
}
    
bool    usb_lock_and_access(t_dongle *usb, t_status status, bool update)
{
    bool                result;

    pthread_mutex_lock(&usb->lock);
    if (usb->status == status)
        result = true;
    else
        result = false;
    if (update)
        usb->status = status;
    pthread_mutex_unlock(&usb->lock);
    return (result);
}
    