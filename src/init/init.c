/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 10:55:55 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/15 18:48:42 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_coder	*init_coder(t_simulation *sim, int id)
{
	t_coder				*coder;

	coder = (t_coder *)malloc(sizeof(t_coder));
	if (!coder)
		return (NULL);
	coder->sim = sim;
	coder->id = id + 1;
	coder->usb_right = NULL;
	coder->status = CODER_INIT;
	coder->completed_compiles = 0;
	coder->last_compile_start = 0;
	coder->has_thread = false;
	coder->has_lock = false;
	if (pthread_mutex_init(&coder->lock, NULL))
	{
		free(coder);
		return (NULL);
	}
	coder->has_lock = true;
	sim->n_coders++;
	return (coder);
}

t_dongle	*init_usb(t_simulation *sim, int id)
{
	t_dongle			*usb;

	usb = (t_dongle *)malloc(sizeof(t_dongle));
	if (!usb)
		return (NULL);
	usb->id = id + 1;
	usb->sim = sim;
	usb->status = AVAILABLE;
	usb->queue[0] = NULL;
	usb->queue[1] = NULL;
	usb->last_compile_time = 0;
	usb->has_lock = false;
	usb->has_cond = false;
	if (!pthread_mutex_init(&usb->lock, NULL))
		usb->has_lock = true;
	if (!pthread_cond_init(&usb->cond, NULL))
		usb->has_cond = true;
	if (usb->has_lock == false || usb->has_cond == false)
		return (NULL);
	sim->n_dongles++;
	return (usb);
}

void	link_coders_and_usbs(t_coder **hub, int number_of_coders)
{
	int					i;
	int					prev_idx;

	if (number_of_coders == 1)
		return ;
	i = 0;
	while (i < number_of_coders)
	{
		if (i == 0)
			prev_idx = number_of_coders - 1;
		else
			prev_idx = i - 1;
		hub[i]->usb_left = hub[prev_idx]->usb_right;
		i++;
	}
}

t_status	init_coworking(t_simulation *sim)
{
	int					i;
	int					n;

	n = sim->number_of_coders;
	sim->quantum = (t_dongle **)malloc(sizeof(t_dongle *) * (n + 1));
	if (!sim->quantum)
		return (MALLOC_ERR);
	sim->hub = (t_coder **)malloc(sizeof(t_coder *) * (n + 1));
	if (!sim->hub)
		return (MALLOC_ERR);
	i = -1;
	while (++i < n)
	{
		sim->hub[i] = init_coder(sim, i);
		if (!sim->hub[i])
			return (MALLOC_ERR);
		sim->quantum[i] = init_usb(sim, i);
		if (!sim->quantum[i])
			return (MALLOC_ERR);
		sim->hub[i]->usb_right = sim->quantum[i];
	}
	sim->hub[i] = NULL;
	sim->quantum[i] = NULL;
	link_coders_and_usbs(sim->hub, n);
	return (SUCCESS);
}
