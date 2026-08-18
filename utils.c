/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 10:38:31 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/18 12:37:13 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int timer(struct timeval *start)
{
    struct timeval      end;
    int                 sec;
    int                 usec;
    int                 msec;

    gettimeofday(&end, NULL);
    sec = end.tv_sec - start->tv_sec;
    usec = end.tv_usec - start->tv_usec;
    msec = (sec * 1000) + (usec / 1000);
    return (msec);
}

int isalldigits(char *str)
{
    if (!str || *str == '\0')
        return (0);
    while (*str)
    {
        if (*str < '0' || *str > '9')
            return (0);
        str++;
    }
    return (1);
}

