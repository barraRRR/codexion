/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 10:38:31 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/22 11:24:55 by jbarreir         ###   ########.fr       */
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
    bool            printable;

    
    pthread_mutex_lock(&coder->lock_status);
    pthread_mutex_lock(&coder->sim->lock_log);
    if (check_completion(coder))
        printable = false;
    else
        printable = true;
    if (coder->status == TAKING_DONGLE && printable)
    printf("%lld %d has taken a dongle\n", timestamp, coder->id);
    else if (coder->status == COMPILING && printable)
        printf("%lld %d is compiling\n", timestamp, coder->id);
    else if (coder->status == DEBUGGING && printable)
        printf("%lld %d is debugging\n", timestamp, coder->id);
    else if (coder->status == REFACTORING && printable)
        printf("%lld %d is refactoring\n", timestamp, coder->id);
    else if (coder->status == BURNOUT && printable)
        printf("%lld %d burned out\n", timestamp, coder->id);
    pthread_mutex_unlock(&coder->lock_status);
    pthread_mutex_unlock(&coder->sim->lock_log);
}

bool    check_completion(t_coder *coder)
{
    pthread_mutex_lock(&coder->sim->lock_status);
    if (coder->sim->status == COMPILING_COMPLETED)
    {
        pthread_mutex_unlock(&coder->sim->lock_status);
        return (true);
    }
    pthread_mutex_unlock(&coder->sim->lock_status);
    return (false);
}
    
void    vigilant_sleep(t_coder *coder, long long sleeping_time)
{
    long long           n_cycles;
    long long           rest;
    
    n_cycles = sleeping_time / SLEEP_INTERVAL;
    rest = sleeping_time % SLEEP_INTERVAL;
    while (n_cycles--)
    {
        usleep(SLEEP_INTERVAL);
        if (check_completion(coder))
            return ;
    }
    usleep(rest);
}