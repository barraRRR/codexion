/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 12:02:13 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/30 10:30:28 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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

static void	print_batch(t_printer *p, t_log *batch, t_log *next)
{
	while (batch)
	{
		next = batch->next;
		if (p->print)
			print_log(batch);
		if (batch->status == BURNOUT)
			p->print = false;
		free(batch);
		batch = next;
	}
}

void	*printer_routine(void *arg)
{
	t_printer			*p;
	t_log				*batch;
	t_log				*next;

	p = (t_printer *)arg;
	next = NULL;
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
			break ;
		print_batch(p, batch, next);
	}
	return (NULL);
}
