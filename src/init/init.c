/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 10:55:55 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/22 10:55:52 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Frees all coder pointers
void    free_hub_memory(t_simulation *sim, int i)
{
    while (i >= 0)
    {
        free(sim->hub[i]);
        free(sim->quantum[i]);
        i--;
    }
    free(sim->hub);
    free(sim->quantum);
}

t_coder *init_coder(t_simulation *sim, int id)
{
    t_coder             *coder;

    coder = (t_coder *)malloc(sizeof(t_coder));
    if (!coder)
        return (NULL);
    coder->sim = sim;
    coder->id = id + 1;
    coder->coder_right = NULL;
    coder->coder_left = NULL;
    coder->usb_right = NULL;
    coder->status = CODER_INIT;
    if (pthread_mutex_init(&coder->lock_status, NULL))
        return (NULL);
    return (coder);
}

t_dongle    *init_usb(int id, t_scheduler scheduler)
{
    t_dongle        *usb;

    usb = (t_dongle*)malloc(sizeof(t_dongle));
    if (!usb)
        return (NULL);
    usb->id = id + 1;
    usb->status = AVAILABLE;
    usb->scheduler = scheduler;
    usb->queue[0] = NULL;
    usb->queue[1] = NULL;
    if (pthread_mutex_init(&usb->lock, NULL))
        return (NULL);
    pthread_cond_init(&usb->cond, NULL);    // Tengo que manejar error aqui?
    return (usb);
}

void    link_coders_and_usbs(t_coder **hub, int number_of_coders)
{
    int             i;
    int             prev_idx;
    int             next_idx;

    i = 0;
    while (i < number_of_coders)
    {
        if (i == 0)
            prev_idx = number_of_coders - 1;
        else
            prev_idx = i - 1;
        next_idx = (i + 1) % number_of_coders;
        hub[i]->coder_left = hub[prev_idx];
        hub[i]->usb_left = hub[prev_idx]->usb_right;
        hub[i++]->coder_right = hub[next_idx];
    }
}

t_status    init_coworking(t_simulation *sim)
{
    int                 i;
    int                 n;

    n = sim->number_of_coders;
    sim->quantum = (t_dongle**)malloc(sizeof(t_dongle*) * (n + 1));
    if (!sim->quantum)
        return (MALLOC_ERR);
    sim->hub = (t_coder **)malloc(sizeof(t_coder *) * (n + 1));
    if (!sim->hub)
        return (MALLOC_ERR);
    i = 0;
    while (i < n)
    {
        sim->hub[i] = init_coder(sim, i);
        if (!sim->hub[i])
            return (MALLOC_ERR);
        sim->quantum[i] = init_usb(i, sim->scheduler);
        if (!sim->quantum[i])
            return (MALLOC_ERR);
        sim->hub[i]->usb_right = sim->quantum[i];
        i++;
    }
    sim->hub[i] = NULL;
    sim->quantum[i] = NULL;
    link_coders_and_usbs(sim->hub, n);
    return (SUCCESS);
}

