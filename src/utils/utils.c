/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 10:38:31 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/22 16:09:07 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	print_err(int error_code, char *err, t_simulation *sim, bool free_mem)
{
	fprintf(stderr, "ERROR: ");
	fprintf(stderr, "%s", err);
	if (free_mem)
		free_hub_memory(sim);
	return (error_code);
}

long long	timer(struct timeval *start)
{
	struct timeval		end;
	long long			sec;
	long long			usec;
	long long			msec;

	gettimeofday(&end, NULL);
	sec = end.tv_sec - start->tv_sec;
	usec = end.tv_usec - start->tv_usec;
	msec = (sec * 1000) + (usec / 1000);
	return (msec);
}

void	print_log(t_coder *coder, long long timestamp, bool lock)
{
	pthread_mutex_lock(&coder->sim->log);
	if (lock)
		pthread_mutex_lock(&coder->lock);
	if (coder->status == TAKING_DONGLE)
		printf("%lld %d has taken a dongle\n", timestamp, coder->id);
	else if (coder->status == COMPILING)
		printf("%lld %d is compiling\n", timestamp, coder->id);
	else if (coder->status == DEBUGGING)
		printf("%lld %d is debugging\n", timestamp, coder->id);
	else if (coder->status == REFACTORING)
		printf("%lld %d is refactoring\n", timestamp, coder->id);
	else if (coder->status == BURNOUT)
		printf("%lld %d burned out\n", timestamp, coder->id);
	if (lock)
		pthread_mutex_unlock(&coder->lock);
	fflush(stdout);
	pthread_mutex_unlock(&coder->sim->log);
}

void	vigilant_sleep(t_coder *coder, long long sleeping_time)
{
	long long			start;

	start = timer(&coder->sim->start);
	while ((timer(&coder->sim->start) - start) < sleeping_time)
	{
		pthread_mutex_lock(&coder->sim->lock);
		if (coder->sim->status == SHUTDOWN_SIGNAL)
		{
			pthread_mutex_unlock(&coder->sim->lock);
			return ;
		}
		pthread_mutex_unlock(&coder->sim->lock);
		usleep(SLEEP_INTERVAL);
	}
}

bool	safe_coder_status(t_coder *coder, t_status status, bool lock)
{
	bool				result;

	if (lock)
		pthread_mutex_lock(&coder->lock);
	if (coder->status == status)
		result = true;
	else
		result = false;
	if (lock)
		pthread_mutex_unlock(&coder->lock);
	return (result);
}
