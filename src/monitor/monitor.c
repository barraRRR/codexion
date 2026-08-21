/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:52:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/21 15:10:00 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    *monitor_routine(void *arg)
{
    t_monitor           *monitor;
    
    monitor = (t_monitor*)arg;
    while(true)
    {
        if (monitor->sim->completed_compiles >= monitor->sim->number_of_compiles_required)
            break ;
        if (monitor->sim->status == ALL_COMPILES_COMPLETED)
            break ;
        if (monitor->burnout == true)
            break ;
        usleep(1000);
    }
    return (monitor);
}