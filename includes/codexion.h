/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:21 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/21 15:06:48 by jbarreir         ###   ########.fr       */
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
# include <stdbool.h>

// *** LIMITS ***
# define MAX_CODERS 512
# define MAX_TIME 9999999999

// *** STATE MACHINE AND ERROR MESSAGES ***
typedef enum e_status
{
    SUCCESS,
    PARSING_COMPLETED,
    SCHED_ERR,
    ARG_COUNT_ERR,
    ARG_NUM_ERR,
    ARG_INT_ERR,
    MIN_COD_ERR,
    THREAD_ERR,
    INIT_THREAD_COMPLETED,
    INIT_THREAD_FAILED,
    MALLOC_ERR,
    BURNOUT,
    FIFO_QUEUE,
    CODER_INIT,
    WAITING_DONGLE,
    TAKING_DONGLE,
    COMPILING,
    COMPILING_COMPLETED,
    DEBUGGING,
    DEBUGGING_COMPLETED,
    REFACTORING,
    REFACTORING_COMPLETED,
    ALL_COMPILES_COMPLETED,
    AVAILABLE,
    PLUGGED,
    COOLING_DOWN
}   t_status;

#define ARG_COUNT_ERR_MSG "Invalid number of arguments\n"
#define ARG_NUM_ERR_MSG "Numerical arguments must be positive numerical arguments\n"
#define ARG_INT_ERR_MSG "Numerical arguments cannot be greater than INT_MAX\n"
#define SCHED_ERR_MSG "Scheduler must be 'fifo' or 'edf'\n"
#define MIN_COD_ERR_MSG "Quantum Compiling needs 2 or more coders\n"
#define THREAD_ERR_MSG "Thread error\n"
#define MALLOC_ERR_MSG "Malloc error\n"

// *** DATA STRUCTURES ***
typedef struct s_dongle t_dongle;
typedef struct s_coder t_coder;
typedef struct s_monitor t_monitor;
typedef struct s_simulation t_simulation;

typedef enum e_scheduler
{
    FIFO,
    EDF
}   t_scheduler;

struct s_dongle
{
    int                     id;
    pthread_mutex_t         lock;
    pthread_cond_t          cond;
    t_status                status;
    t_scheduler             scheduler;
    t_coder                 *queue[2];
    long long               dongle_cooldown;
    long long               last_compile_time;
};

struct s_coder
{
    t_simulation            *sim;
    pthread_t               thread;
    int                     id;
    t_status                status;
    struct s_coder          *coder_right;
    t_dongle                *usb_right;
    struct s_coder          *coder_left;
    t_dongle                *usb_left;
    long long               last_compile_time;
};

struct s_monitor
{
    t_simulation            *sim;
    pthread_t               thread;
    bool                    burnout;
};

struct s_simulation
{
    t_monitor               monitor;
    t_coder                 **hub;
    t_dongle                **quantum;
    struct timeval          start;
    t_status                status;
    int                     number_of_coders;
    long long               time_to_burnout;
    long long               time_to_compile;
    long long               time_to_debug;
    long long               time_to_refactor;
    int                     number_of_compiles_required;
    long long               dongle_cooldown;
    int                     completed_compiles;
    pthread_mutex_t         lock_compiles;
    pthread_mutex_t         lock_log;
    t_scheduler             scheduler;
};


// *** PROTOTYPES ***
long long       timer(struct timeval *start);
int             parse_rules(t_simulation *sim, int argc, char **argv);
t_status        init_coworking(t_simulation *sim);
void            free_hub_memory(t_simulation *sim, int i);
int             init_threads(t_simulation *sim);
void            join_threads(t_coder **hub, int number_of_coders);
int             print_err(int error_code, char *err);
void            print_log(t_coder *coder, long long timestamp);
void            *quantum_compiler(void *arg);
void            print_status(t_simulation sim);
void            fifo_scheduler(t_coder **queue, t_coder *coder);
void            edf_scheduler(t_coder **queue, t_coder *coder);
t_coder         *dequeue(t_coder **queue);
void            enqueue_coder(t_coder *coder);
void            *monitor_routine(void *arg);

#endif