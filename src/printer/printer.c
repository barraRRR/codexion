/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 12:02:13 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/29 11:58:03 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_log	*create_log(long long time, t_status status, int coder_id)
{
	t_log				*new_log;

	new_log = (t_log *)malloc(sizeof(t_log));
	if (!new_log)
		return (NULL);
	new_log->time = time;
	new_log->next = NULL;
	new_log->status = status;
	new_log->coder_id = coder_id;
	return (new_log);
}

bool	append_log(t_coder *coder, long long time)
{
	t_log				*log;
	t_printer			*p;

	log = create_log(time, coder->status, coder->id);
	if (!log)
		return (false);
	p = &coder->sim->printer;
	pthread_mutex_lock(&p->lock);
	if (p->tail)
		p->tail->next = log;
	else
		p->head = log;
	p->tail = log;
	pthread_cond_broadcast(&p->cond);
	pthread_mutex_unlock(&p->lock);
	return (true);
}

void	free_logs(t_printer *printer)
{
	t_log				*ptr;
	t_log				*next;

	ptr = printer->head;
	next = NULL;
	while (ptr)
	{
		next = ptr->next;
		free(ptr);
		ptr = next;
	}
}

static void	print_log(t_log *log)
{
	if (log->status == TAKING_DONGLE)
	{
		printf("%lld %d has taken a dongle\n", log->time, log->coder_id);
		fflush(stdout);
		printf("%lld %d has taken a dongle\n", log->time, log->coder_id);
	}
	else if (log->status == COMPILING)
		printf("%lld %d is compiling\n", log->time, log->coder_id);
	else if (log->status == DEBUGGING)
		printf("%lld %d is debugging\n", log->time, log->coder_id);
	else if (log->status == REFACTORING)
		printf("%lld %d is refactoring\n", log->time, log->coder_id);
	else if (log->status == BURNOUT)
		printf("%lld %d burned out\n", log->time, log->coder_id);
	fflush(stdout);
}

void	*printer_routine(void *arg)
{
	t_printer			*p;
	t_log				*batch;
	t_log				*next;

	p = (t_printer *)arg;
	wait_for_start_sequence(p->sim);
	while (true)
	{
		pthread_mutex_lock(&p->lock);
		while (!p->head && !p->stop)
			pthread_cond_wait(&p->cond, &p->lock);
		batch = p->head;
		p->head = NULL;
		p->tail = NULL;
		pthread_mutex_unlock(&p->lock);
		if (!batch)
			break;
		while (batch)
		{
			next = batch->next;
			print_log(batch);
			free(batch);
			batch = next;
		}
	}
	return (NULL);
}
