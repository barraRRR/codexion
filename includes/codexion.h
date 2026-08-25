/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:21 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/25 12:16:02 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

// *** INCLUDES ***
# include <pthread.h>
# include <stdio.h>
# include <strings.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <limits.h>
# include <stdbool.h>

// *** LIMITS ***
# define MAX_CODERS 512
# define MAX_TIME 9999999999
# define SLEEP_INTERVAL 1000
# define MONITOR_SLEEP 1000

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
    INIT_DONGLES,
    INIT_DONGLES_ERR,
    INIT_DONGLES_COMPLETED,
    INIT_THREADS,
    INIT_THREADS_COMPLETED,
    INIT_THREADS_ERR,
    INIT_SIMULATION,
    INIT_SIMULATION_ERR,
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
    AVAILABLE_LEFT,
    AVAILABLE_RIGHT,
    AVAILABLE_BOTH,
    AVAILABLE_NONE,
    PLUGGED,
    COOLING_DOWN,
    SHUTDOWN_SIGNAL
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
    t_simulation            *sim;
    pthread_mutex_t         lock;
    pthread_cond_t          cond;
    int                     id;
    t_status                status;
    t_coder                 *queue[2];
    long long               last_compile_time;
};

struct s_coder
{
    t_simulation            *sim;
    pthread_t               thread;
    pthread_mutex_t         lock;
    int                     id;
    t_status                status;
    t_dongle                *usb_right;
    t_dongle                *usb_left;
    int                     completed_compiles;
    long long               last_compile_time;
};

struct s_monitor
{
    t_simulation            *sim;
    pthread_t               thread;
};

struct s_simulation
{
    pthread_mutex_t         lock;
    pthread_mutex_t         log;
    t_coder                 **hub;
    t_dongle                **quantum;
    t_monitor               monitor;
    t_status                status;
    struct timeval          start;
    int                     number_of_coders;
    long long               time_to_burnout;
    long long               time_to_compile;
    long long               time_to_debug;
    long long               time_to_refactor;
    long long               dongle_cooldown;
    int                     compiles_required;
    t_scheduler             scheduler;
};


// *** PROTOTYPES ***
long long       timer(struct timeval *start);
int             parse_rules(t_simulation *sim, int argc, char **argv);
t_status        init_coworking(t_simulation *sim);
void            free_hub_memory(t_simulation *sim, int i);
t_status             init_threads(t_simulation *sim);
void join_threads_and_destroy_mutex_cond(t_simulation *sim);
int             print_err(int error_code, char *err);
void            print_log(t_coder *coder, long long timestamp, bool lock);
void            *quantum_compiler(void *arg);
void            print_status(t_simulation sim);
void            fifo_scheduler(t_coder **queue, t_coder *coder);
void            edf_scheduler(t_coder **queue, t_coder *coder);
t_coder         *dequeue(t_coder **queue);
void            enqueue_coder(t_coder *coder);
void            *monitor_routine(void *arg);
void            vigilant_sleep(t_coder *coder, long long sleeping_time);

void update_cooldown(t_dongle *usb, struct timeval *start);
bool usb_access(t_dongle *usb, t_coder *coder);
t_status both_usb_access(t_coder *coder);
void lock_dongles_in_order(t_coder *coder);
void unlock_dongles(t_coder *coder);
void take_dongle(t_coder *coder);

void compile_init(t_coder *coder);
void debug_init(t_coder *coder);
void refactor_init(t_coder *coder);
void *quantum_compiler(void *arg);

bool sim_lock_and_access(t_simulation *sim, t_status status, bool update);
bool coder_lock_and_access(t_coder *coder, t_status status, bool update, bool unlock);
bool usb_lock_and_access(t_dongle *usb, t_status status, bool update);

void add_simulation(t_simulation *sim);

bool is_burnout(t_coder *coder);

#endif