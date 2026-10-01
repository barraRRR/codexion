/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:21 by jbarreir          #+#    #+#             */
/*   Updated: 2026/10/01 10:33:05 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# define _POSIX_C_SOURCE 199309L

/* *** INCLUDES *** */
# include <pthread.h>
# include <stdio.h>
# include <strings.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <limits.h>
# include <stdbool.h>

/* *** LIMITS *** */
# define MAX_CODERS 1024
# define SLEEP_INTERVAL 1000
# define MONITOR_SLEEP 1000
# define MONITOR_CHUNK 30

/* *** STATE MACHINE AND ERROR MESSAGES *** */
typedef enum e_status
{
	SUCCESS,
	WAITING,
	PARSING_COMPLETED,
	SCHED_ERR,
	ARG_COUNT_ERR,
	ARG_NUM_ERR,
	MAX_COD_ERR,
	THREAD_ERR,
	INIT_LOG_ERR,
	INIT_SIM_LOCK_ERR,
	INIT_MONITOR_ERR,
	INIT_DONGLES,
	INIT_DONGLES_ERR,
	INIT_DONGLES_COMPLETED,
	INIT_THREADS,
	INIT_THREADS_COMPLETED,
	INIT_THREADS_ERR,
	INIT_START_COND,
	INIT_START_COND_ERR,
	INIT_PRINTER,
	INIT_PRINTER_ERR,
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
	COMPLETION,
	AVAILABLE,
	AVAILABLE_LEFT,
	AVAILABLE_RIGHT,
	AVAILABLE_BOTH,
	AVAILABLE_NONE,
	PLUGGED,
	COOLING_DOWN,
	SHUTDOWN
}	t_status;

# define ARG_COUNT_ERR_MSG "Invalid number of arguments\n"
# define ARG_NUM_ERR_MSG "Numeric args must be an integer in [0, INT_MAX]\n"
# define SCHED_ERR_MSG "Scheduler must be 'fifo' or 'edf'\n"
# define MAX_COD_ERR_MSG "N coders must be in [1, MAX_CODERS (1024 default)]\n"
# define THREAD_ERR_MSG "Thread error\n"
# define MALLOC_ERR_MSG "Malloc error\n"
# define INIT_SIMULATION_ERR_MSG "Simulation mutex initialization error\n"

/* *** DATA STRUCTURES *** */
typedef struct s_dongle		t_dongle;
typedef struct s_coder		t_coder;
typedef struct s_monitor	t_monitor;
typedef struct s_submonitor	t_submonitor;
typedef struct s_log		t_log;
typedef struct s_printer	t_printer;
typedef struct s_simulation	t_simulation;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

struct s_dongle
{
	t_simulation			*sim;
	pthread_mutex_t			lock;
	bool					has_lock;
	pthread_cond_t			cond;
	bool					has_cond;
	int						id;
	t_status				status;
	t_coder					*queue[2];
	long long				last_compile_time;
};

struct s_coder
{
	t_simulation			*sim;
	pthread_t				thread;
	bool					has_thread;
	pthread_mutex_t			lock;
	bool					has_lock;
	int						id;
	t_status				status;
	t_dongle				*usb_right;
	t_dongle				*usb_left;
	int						completed_compiles;
	long long				last_compile_start;
};

struct s_monitor
{
	t_simulation			*sim;
	pthread_t				thread;
	bool					has_thread;
	t_submonitor			**pool;
	int						n_sub;
};

struct s_submonitor
{
	int						id;
	int						i_start;
	int						i_end;
	t_monitor				*m;
	t_simulation			*sim;
	pthread_t				thread;
	bool					has_thread;
	pthread_mutex_t			lock;
	bool					has_lock;
	t_status				status;
	int						n_coders;
};

struct s_log
{
	long long				time;
	t_status				status;
	int						coder_id;
	t_log					*next;
};

struct s_printer
{
	t_simulation			*sim;
	pthread_t				thread;
	bool					has_thread;
	pthread_mutex_t			lock;
	bool					has_lock;
	pthread_cond_t			cond;
	bool					has_cond;
	bool					stop;
	bool					print;
	t_log					*head;
	t_log					*tail;
};

struct s_simulation
{
	pthread_mutex_t			lock;
	bool					has_lock;
	pthread_mutex_t			start_lock;
	bool					has_start_lock;
	pthread_cond_t			start_cond;
	bool					has_start_cond;
	bool					is_started;
	t_coder					**hub;
	t_dongle				**quantum;
	t_monitor				monitor;
	t_printer				printer;
	t_status				status;
	struct timeval			start_time;
	int						n_coders;
	long long				time_to_burnout;
	long long				time_to_compile;
	long long				time_to_debug;
	long long				time_to_refactor;
	long long				dongle_cooldown;
	int						compiles_required;
	t_scheduler				scheduler;
	int						n_dongles_init;
	int						n_coders_init;
};

/* *** PROTOTYPES *** */
long long		timer(t_simulation *sim);
t_status		parse_rules(t_simulation *sim, int argc, char **argv);
t_status		init_coworking(t_simulation *sim);
t_status		free_all_memory(t_simulation *sim, t_status status);
t_status		init_threads(t_simulation *sim);
int				print_err(int error_code, char *err, t_simulation *sim,
					bool free_mem);
t_status		exit_code_unlock(t_coder *coder, t_status status, bool dongles);

void			init_sim_data(t_simulation *sim);
t_submonitor	**create_pool(t_simulation *sim);

void			*quantum_compiler(void *arg);
void			vigilant_sleep(t_coder *coder, long long sleeping_time);
void			fifo_scheduler(t_coder **queue, t_coder *coder);
void			edf_scheduler(t_coder **queue, t_coder *coder);
t_coder			*dequeue(t_coder **queue);
void			enqueue_coder(t_coder *coder);
void			*monitor_routine(void *arg);
void			*submonitor_routine(void *arg);

void			update_cooldown(t_dongle *usb, t_simulation *sim);
t_status		both_usb_access(t_coder *coder);
void			lock_dongles_in_order(t_coder *coder);
void			unlock_dongles(t_coder *coder);
void			dongle_cooldown(t_coder *coder);
void			dequeue_dongles(t_coder *coder);
void			wake_dongles(t_monitor *monitor);

void			wait_for_start_sequence(t_simulation *sim);
void			start_sequence(t_simulation *sim);
t_status		abort_start_sequence(t_simulation *sim, t_status status);

bool			sim_lock_and_access(t_simulation *sim, t_status status,
					bool update);

void			*solo_coder(t_coder *coder);
void			*exit_routine(t_coder *coder);
t_status		exit_code_unlock(t_coder *coder, t_status status, bool dongles);
bool			am_i_burnt(t_coder *coder);
void			have_i_finished(t_coder *coder);
bool			up_and_running(t_coder *coder);

void			*printer_routine(void *arg);
bool			append_log(t_coder *coder);
void			free_logs(t_printer *printer);

void			pthread_destroy_hub(t_simulation *sim);
void			pthread_destroy_dongle(t_simulation *sim);
void			pthread_destroy_monitor(t_monitor *monitor);
void			pthread_destroy_printer(t_printer *printer);
void			pthread_destroy_sim(t_simulation *sim);

#endif