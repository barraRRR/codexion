/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 11:48:29 by jbarreir          #+#    #+#             */
/*   Updated: 2026/10/01 10:49:04 by jbarreir         ###   ########.fr       */
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

static void	sub_assignment(t_submonitor *sub, int *chunk, int n_coders)
{
	sub->i_start = *chunk;
	if (*chunk + MONITOR_CHUNK > n_coders)
		sub->i_end = n_coders - 1;
	else
		sub->i_end = *chunk + MONITOR_CHUNK - 1;
	*chunk += MONITOR_CHUNK;
	sub->n_coders = sub->i_end - sub->i_start + 1;
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
		sub_assignment(pool[i], &chunk, sim->n_coders);
	}
	pool[i] = NULL;
	sim->monitor.n_sub = n_sub;
	return (pool);
}

void	wake_dongles(t_monitor *monitor)
{
	int					i;

	i = -1;
	while (++i < monitor->sim->n_coders)
	{
		pthread_mutex_lock(&monitor->sim->quantum[i]->lock);
		pthread_cond_broadcast(&monitor->sim->quantum[i]->cond);
		pthread_mutex_unlock(&monitor->sim->quantum[i]->lock);
	}
}
