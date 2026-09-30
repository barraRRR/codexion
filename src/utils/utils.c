/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 10:38:31 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/30 09:56:37 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	print_err(int error_code, char *err, t_simulation *sim, bool free_mem)
{
	fprintf(stderr, "ERROR: ");
	fprintf(stderr, "%s", err);
	if (free_mem)
		free_all_memory(sim, error_code);
	return (error_code);
}

long long	timer(t_simulation *sim)
{
	struct timeval		end;
	long long			sec;
	long long			usec;
	long long			msec;

	gettimeofday(&end, NULL);
	sec = end.tv_sec - sim->start_time.tv_sec;
	usec = end.tv_usec - sim->start_time.tv_usec;
	msec = (sec * 1000) + (usec / 1000);
	return (msec);
}

void	vigilant_sleep(t_coder *coder, long long sleeping_time)
{
	long long			start;

	start = timer(coder->sim);
	while ((timer(coder->sim) - start) < sleeping_time)
	{
		pthread_mutex_lock(&coder->sim->lock);
		if (coder->sim->status == SHUTDOWN)
		{
			pthread_mutex_unlock(&coder->sim->lock);
			return ;
		}
		pthread_mutex_unlock(&coder->sim->lock);
		usleep(SLEEP_INTERVAL);
	}
}

bool	sim_lock_and_access(t_simulation *sim, t_status status, bool update)
{
	bool				result;

	pthread_mutex_lock(&sim->lock);
	if (sim->status == status)
		result = true;
	else
		result = false;
	if (update && sim->status != SHUTDOWN)
		sim->status = status;
	pthread_mutex_unlock(&sim->lock);
	return (result);
}

void	update_cooldown(t_dongle *usb, t_simulation *sim)
{
	long long			now;

	pthread_mutex_lock(&usb->lock);
	if (usb->status == COOLING_DOWN)
	{
		now = timer(sim);
		if (now >= usb->last_compile_time + usb->sim->dongle_cooldown)
		{
			usb->status = AVAILABLE;
			pthread_cond_broadcast(&usb->cond);
		}
	}
	pthread_mutex_unlock(&usb->lock);
}
