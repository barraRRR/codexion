/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   awareness.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 15:49:46 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/22 17:44:14 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	am_i_burnt(t_coder *coder)
{
	long long			now;
	bool				result;

	if (coder->status == BURNOUT)
		return (true);
	now = timer(&coder->sim->start);
	result = false;
	if (coder->last_compile_start + coder->sim->time_to_burnout <= now)
	{
		coder->status = BURNOUT;
		result = true;
	}
	return (result);
}

void	have_i_finished(t_coder *coder)
{
	int					completed;
	int					required;

	pthread_mutex_lock(&coder->lock);
	completed = coder->completed_compiles;
	required = coder->sim->compiles_required;
	if (!am_i_burnt(coder))
	{
		if (completed >= required)
			coder->status = ALL_COMPILES_COMPLETED;
		else
			coder->status = REFACTORING_COMPLETED;
	}
	pthread_mutex_unlock(&coder->lock);
}

bool	up_and_running(t_coder *coder)
{
	bool				result;

	result = true;
	if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
		result = false;
	pthread_mutex_lock(&coder->lock);
	if (am_i_burnt(coder))
		result = false;
	pthread_mutex_unlock(&coder->lock);
	return (result);
}
