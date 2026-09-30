/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   submonitor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 11:48:29 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/30 13:56:19 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_submonitor	*create_submonitor(t_monitor *m, int id)
{
	t_submonitor		*sub;

	sub = (t_submonitor *)malloc(sizeof(t_submonitor));
	if (!sub)
		return (NULL);
	sub->id = id;
	sub->m = m;
	sub->sim = m->sim;
	sub->has_thread = false;
	sub->has_lock = false;
	sub->status = WAITING;
	if (pthread_mutex_init(&sub->lock, NULL))
	{
		free(sub);
		return (NULL);
	}
	sub->has_lock = true;
	return (sub);
}

static int	calculate_n_sub(t_simulation *sim)
{
	int					result;

	if (MONITOR_CHUNK <= 0)
		return (0);
	result = sim->n_coders / MONITOR_CHUNK;
	if (sim->n_coders % MONITOR_CHUNK != 0)
		result++;
	return (result);
}

t_submonitor	**create_pool(t_simulation *sim)
{
	int					chunk;
	int					n_sub;
	int					i;
	t_submonitor		**pool;

	n_sub = calculate_n_sub(sim);
	pool = (t_submonitor **)malloc(sizeof(t_submonitor *) * (n_sub + 1));
	if (!pool)
		return (NULL);
	chunk = 0;
	i = -1;
	while (++i < n_sub)
	{
		pool[i] = create_submonitor(&sim->monitor, i + 1);
		if (!pool[i])
			return (NULL);
		pool[i]->i_start = chunk;
		if (chunk + MONITOR_CHUNK > sim->n_coders)
			pool[i]->i_end = sim->n_coders - 1;
		else
			pool[i]->i_end = chunk + MONITOR_CHUNK - 1;
		chunk += MONITOR_CHUNK;
	}
	pool[i] = NULL;
	sim->monitor.n_sub = n_sub;
	return (pool);
}

void	wake_dongles(t_monitor *monitor)
{
	int i;

	i = -1;
	while (++i < monitor->sim->n_coders)
	{
		pthread_mutex_lock(&monitor->sim->quantum[i]->lock);
		pthread_cond_broadcast(&monitor->sim->quantum[i]->cond);
		pthread_mutex_unlock(&monitor->sim->quantum[i]->lock);
	}
}
