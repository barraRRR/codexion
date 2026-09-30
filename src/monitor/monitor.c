/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:52:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/30 13:57:08 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_status	check_coder(t_coder *coder)
{
	t_status			status;

	status = COMPILING;
	pthread_mutex_lock(&coder->lock);
	if (coder->status == COMPLETION)
		status = COMPLETION;
	else if (coder->status == BURNOUT || am_i_burnt(coder))
	{
		status = BURNOUT;
		if (!append_log(coder))
			status = MALLOC_ERR;
	}
	pthread_mutex_unlock(&coder->lock);
	return (status);
}

static t_status	check_hub(t_simulation *sim, t_submonitor *sub)
{
	int					i;
	int					active;
	t_status			status;

	i = sub->i_start - 1;
	active = 0;
	while (++i < sub->i_end)
	{
		update_cooldown(sim->quantum[i], sim);
		status = check_coder(sim->hub[i]);
		if (status == BURNOUT || status == MALLOC_ERR)
			return (SHUTDOWN);
		if (status == COMPILING)
			active++;
	}
	if (active == 0)
		return (COMPLETION);
	return (COMPILING);
}

static t_status	check_pool(t_monitor *mon)
{
	int					i;
	int					completion;
	t_status			status;

	completion = 0;
	i = -1;
	while (++i < mon->n_sub)
	{
		pthread_mutex_lock(&mon->pool[i]->lock);
		status = mon->pool[i]->status;
		pthread_mutex_unlock(&mon->pool[i]->lock);
		if (status == BURNOUT)
			return (BURNOUT);
		if (status == COMPLETION)
			completion++;
	}
	if (completion == mon->n_sub)
		return (COMPLETION);
	return (COMPILING);
}

void	*submonitor_routine(void *arg)
{
	t_submonitor		*sub;

	sub = (t_submonitor *)arg;
	wait_for_start_sequence(sub->sim);
	while (true)
	{
		pthread_mutex_lock(&sub->lock);
		sub->status = check_hub(sub->sim, sub);
		if (sub->status == SHUTDOWN || sub->status == COMPLETION)
		{
			pthread_mutex_lock(&sub->lock);
			break ;
		}
		pthread_mutex_lock(&sub->lock);
		usleep(MONITOR_SLEEP);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_monitor *mon;
	t_status s;

	mon = (t_monitor *)arg;
	wait_for_start_sequence(mon->sim);
	while (true)
	{
		s = check_pool(mon);
		if (s == BURNOUT || s == COMPLETION)
		{
			sim_lock_and_access(mon->sim, SHUTDOWN, true);
			break;
		}
		usleep(MONITOR_SLEEP);
	}
	wake_dongles(mon);
	return (NULL);
}
