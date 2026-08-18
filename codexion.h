/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:21 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/18 12:44:01 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

// *** INCLUDES ***
# include <pthread.h>
# include <stdio.h>
# include <strings.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <limits.h>

// *** LIMITS ***
# define MAX_CODERS 512
//# define MAX_TIME 9999999999

// *** ENUM ***
typedef enum e_scheduler
{
    FIFO,
    EDF
}   t_scheduler;

typedef enum e_state
{
    WAITING = 0,
    COMPILING,
    DEBUGGING,
    REFACTORING,
    BURNOUT
}   t_state;

// *** SIMULATION STATUS ***
typedef enum e_sim_status
{
    PARSING_COMPLETED,
    RUNNING
}   t_sim_status;

typedef enum e_error_code
{
    SUCCESS,
    SCHED_ERR,
    ARG_COUNT_ERR,
    ARG_NUM_ERR
} t_error_code;

#define ARG_COUNT_ERR_MSG "ERROR: Invalid number of arguments\n"
#define ARG_NUM_ERR_MSG "ERROR: Numerical arguments must be positive numerical arguments\n"
#define SCHED_ERR_MSG "ERROR: Scheduler must be 'fifo' or 'edf'\n"

// *** STRUCT ***
typedef struct s_coder
{
    pthread_t               c_pthread;
    t_state                 state;
    struct s_coder          *right;
    struct s_coder          *left;
}   t_coder;

typedef struct s_monitor
{
    struct timeval          start;
    int                     status;
    int                     number_of_coders;
    int                     time_to_burnout;
    int                     time_to_compile;
    int                     time_to_debug;
    int                     time_to_refactor;
    int                     number_of_compiles_required;
    int                     dongle_cooldown;
    t_scheduler             scheduler;
}   t_monitor;

// *** PROTOTYPE ***
int             timer(struct timeval *start);
int             isalldigits(char *str);

#endif