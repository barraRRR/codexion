/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 12:02:13 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/28 19:59:40 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	init_printer(t_printer *printer, t_simulation *sim)
{
	printer->sim = sim;
	printer->has_lock = false;
	printer->has_cond = false;
	printer->print = false;
	printer->head_log = NULL;
	printer->last_printed = NULL;
	if (!pthread_mutex_init(&printer->lock, NULL))
		printer->has_lock = true;
	if (!pthread_cond_init(&printer->cond, NULL))
		printer->has_cond = true;
	return (printer->has_lock && printer->has_cond);
}

t_log	*create_log(long long time, t_status status, int coder_id)
{
	t_log				*new_log;

	new_log = (t_log *)malloc(sizeof(t_log));
	if (!new_log)
		return (NULL);
	new_log->time = time;
	new_log->status = status;
	new_log->coder_id = coder_id;
	return (new_log);
}

bool	append_log(t_coder *coder, long long time)
{
	t_log				*new_log;
	t_log				*ptr;

	new_log = create_log(time, coder->status, coder->id);
	if (!new_log)
		return (false);
	pthread_mutex_lock(&coder->sim->printer.lock);
	ptr = coder->sim->printer.head_log;
	if (!ptr)
		coder->sim->printer.head_log = new_log;
	else
	{
		while (ptr->next)
			ptr = ptr->next;
		ptr->next = new_log;
	}
	coder->sim->printer.print = true;
	pthread_cond_broadcast(&coder->sim->printer.cond);
	pthread_mutex_unlock(&coder->sim->printer.lock);
	return (true);
}

void	free_logs(t_printer *printer, bool free_all)
{
	t_log				*ptr;
	t_log				*next;

	ptr = printer->head_log;
	next = NULL;
	while (ptr)
	{
		if (!free_all && ptr == printer->last_printed)
		{
			free(ptr);
			printer->head_log = NULL;
			printer->last_printed = NULL;
			break ;
		}
		if (ptr->next)
			next = ptr->next;
		free(ptr);
		if (next)
			ptr = next;
	}
}

static void	dequeue_logs(t_printer *printer)
{
	pthread_mutex_lock(&printer->lock);
	free_logs(printer, false);
	printer->head_log = printer->last_printed;
	pthread_mutex_unlock(&printer->lock);
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
	fflush(stdout);
}

static void	print_log_sequence(t_printer *printer)
{
	t_log				*ptr;

	ptr = printer->head_log;
	while (ptr)
	{
		print_log(ptr);
		printer->last_printed = ptr;
		ptr = ptr->next;
	}
}

void	*printer_routine(void *arg)
{
	t_printer			*printer;

	printer = (t_printer *)arg;
	wait_for_start_sequence(printer->sim);
	while (printer->head_log
			|| !sim_lock_and_access(printer->sim, SHUTDOWN, false))
	{
		pthread_mutex_lock(&printer->lock);
		while (!printer->print)
			pthread_cond_wait(&printer->cond, &printer->lock);
		pthread_mutex_unlock(&printer->lock);
		print_log_sequence(printer);
		dequeue_logs(printer);
	}
	return (NULL);
}
