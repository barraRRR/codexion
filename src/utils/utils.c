/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 10:38:31 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/21 13:34:09 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int print_err(int error_code, char *err)
{
    fprintf(stderr, "ERROR: ");
    fprintf(stderr, "%s", err);
    return (error_code);
}

long long   timer(struct timeval *start)
{
    struct timeval      end;
    long long                 sec;
    long long                 usec;
    long long                 msec;

    gettimeofday(&end, NULL);
    sec = end.tv_sec - start->tv_sec;
    usec = end.tv_usec - start->tv_usec;
    msec = (sec * 1000) + (usec / 1000);
    return (msec);
}

void    print_log(t_coder *coder, long long timestamp)
{
    pthread_mutex_lock(&coder->sim->lock_log);
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
    else
        printf("%lld %d undefined beheaviour", timestamp, coder->id);
    pthread_mutex_unlock(&coder->sim->lock_log);
}
