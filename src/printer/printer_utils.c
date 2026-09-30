/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 10:05:08 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/30 11:02:27 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_log	*create_log(t_status status, int coder_id)
{
	t_log				*new_log;

	new_log = (t_log *)malloc(sizeof(t_log));
	if (!new_log)
		return (NULL);
	new_log->time = 0;
	new_log->next = NULL;
	new_log->status = status;
	new_log->coder_id = coder_id;
	return (new_log);
}

bool	append_log(t_coder *coder)
{
	t_log				*log;
	t_printer			*p;

	log = create_log(coder->status, coder->id);
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
