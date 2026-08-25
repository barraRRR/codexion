/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 10:38:31 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/24 17:46:46 by jbarreir         ###   ########.fr       */
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

void    print_log(t_coder *coder, long long timestamp, bool lock)
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

bool    check_completion(t_coder *coder)
{
    bool                completed;

    pthread_mutex_lock(&coder->sim->lock);
    completed = (coder->sim->status == COMPILING_COMPLETED || coder->sim->status == BURNOUT);
    pthread_mutex_unlock(&coder->sim->lock);
    return (completed);
}
    
void    vigilant_sleep(t_coder *coder, long long sleeping_time)
{
    long long           start;

    start = timer(&coder->sim->start);
    while ((timer(&coder->sim->start) - start) < sleeping_time)
    {
        if (check_completion(coder))
            return ;
        usleep(SLEEP_INTERVAL);
    }
}

void    add_simulation(t_simulation *sim)
{
    pthread_mutex_lock(&sim->lock);
    (sim->completed_compiles)++;
    pthread_mutex_unlock(&sim->lock);
}

bool    is_burnout(t_coder *coder)
{
    return (coder->status == BURNOUT);
}