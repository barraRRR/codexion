/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 10:29:43 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/22 17:05:59 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	lock_and_wait(t_coder *coder, t_dongle *left, t_dongle *right,
						t_status status)
{
	pthread_mutex_t		*available;
	pthread_mutex_t		*locked;
	pthread_cond_t		*cond;

	available = &right->lock;
	locked = &left->lock;
	cond = &left->cond;
	if (status == AVAILABLE_LEFT || status == AVAILABLE_NONE)
	{
		available = &left->lock;
		locked = &right->lock;
		cond = &right->cond;
	}
	pthread_mutex_unlock(&coder->lock);
	pthread_mutex_unlock(available);
	pthread_cond_wait(cond, locked);
	pthread_mutex_unlock(locked);
	pthread_mutex_lock(&coder->lock);
	lock_dongles_in_order(coder);
}

void	take_dongle(t_coder *coder)
{
	t_status			available;

	lock_dongles_in_order(coder);
	available = both_usb_access(coder);
	while (available != AVAILABLE_BOTH)
	{
		lock_and_wait(coder, coder->usb_left, coder->usb_right, available);
		available = both_usb_access(coder);
		if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
		{
			unlock_dongles(coder);
			pthread_mutex_unlock(&coder->lock);
			return ;
		}
	}
	if (am_i_burnt(coder, false, true, TAKING_DONGLE))
		return ;
	print_log(coder, timer(&coder->sim->start), false);
	coder->usb_right->status = PLUGGED;
	print_log(coder, timer(&coder->sim->start), false);
	coder->usb_left->status = PLUGGED;
	dequeue(coder->usb_right->queue);
	dequeue(coder->usb_left->queue);
	unlock_dongles(coder);
	pthread_mutex_unlock(&coder->lock);
}

void	compile_init(t_coder *coder)
{
	long long			time;

	if (am_i_burnt(coder, false, true, COMPILING))
		return ;
	time = timer(&coder->sim->start);
	coder->last_compile_start = time;
	print_log(coder, timer(&coder->sim->start), false);
	pthread_mutex_unlock(&coder->lock);
	vigilant_sleep(coder, coder->sim->time_to_compile);
	if (safe_coder_status(coder, BURNOUT, true))
		return ;
	pthread_mutex_lock(&coder->lock);
	lock_dongles_in_order(coder);
	coder->completed_compiles++;
	if (am_i_burnt(coder, false, true, COMPILING_COMPLETED))
		return;
	pthread_mutex_unlock(&coder->lock);
	coder->usb_right->last_compile_time = time;
	coder->usb_left->last_compile_time = time;
	coder->usb_right->status = COOLING_DOWN;
	coder->usb_left->status = COOLING_DOWN;
	unlock_dongles(coder);
}

void	debug_and_refactor(t_coder *coder)
{
	if (am_i_burnt(coder, false, true, DEBUGGING))
		return ;
	print_log(coder, timer(&coder->sim->start), false);
	pthread_mutex_unlock(&coder->lock);
	vigilant_sleep(coder, coder->sim->time_to_debug);
	if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
		return ;
	pthread_mutex_lock(&coder->lock);
	if (am_i_burnt(coder, false, true, REFACTORING))
		return ;
	print_log(coder, timer(&coder->sim->start), false);
	pthread_mutex_unlock(&coder->lock);
	vigilant_sleep(coder, coder->sim->time_to_refactor);
	if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
		return ;
	have_i_finished(coder);
}

bool	up_and_running(t_coder *coder)
{
	if (sim_lock_and_access(coder->sim, SHUTDOWN_SIGNAL, false))
		return (false);
	if (am_i_burnt(coder, true, false, 0))
		return (false);
	return (true);
}

void	*quantum_compiler(void *arg)
{
	t_coder				*coder;
	t_status			status;

	coder = (t_coder *)arg;
	if (coder->sim->number_of_coders == 1)
		return (solo_coder(coder));
	while (true)
	{
		if (!up_and_running(coder))
			break ;
		pthread_mutex_lock(&coder->lock);
		status = coder->status;
		if (status == BURNOUT || status == ALL_COMPILES_COMPLETED)
			return (exit_routine(coder));
		if (status == CODER_INIT || status == REFACTORING_COMPLETED)
			enqueue_coder(coder);
		else if (status == WAITING_DONGLE)
			take_dongle(coder);
		else if (status == TAKING_DONGLE)
			compile_init(coder);
		else if (status == COMPILING_COMPLETED)
			debug_and_refactor(coder);
	}
	return (NULL);
}
