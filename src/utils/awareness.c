/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   awareness.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 15:49:46 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/22 17:04:43 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	am_i_burnt(t_coder *coder, bool lock, bool update, t_status status)
{
	long long			now;
	bool				result;

	now = timer(&coder->sim->start);
	result = false;
	if (lock)
		pthread_mutex_lock(&coder->lock);
	if (coder->last_compile_start + coder->sim->time_to_burnout <= now)
	{
		coder->status = BURNOUT;
		result = true;
	}
	else if (update)
		coder->status = status;
	if (lock)
		pthread_mutex_unlock(&coder->lock);
	return (result);
}

void	have_i_finished(t_coder *coder)
{
	int					completed;
	int					required;

	pthread_mutex_lock(&coder->lock);
	completed = coder->completed_compiles;
	required = coder->sim->compiles_required;
	if (!am_i_burnt(coder, false, false, 0))
	{
		if (completed >= required)
			coder->status = ALL_COMPILES_COMPLETED;
		else
			coder->status = REFACTORING_COMPLETED;
	}
	pthread_mutex_unlock(&coder->lock);
}