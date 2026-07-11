#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

// Stores the scheduler type (FIFO or EDF).
typedef enum e_scheduler
{
	SCHED_POLICY_FIFO,
	SCHED_POLICY_EDF
}	t_scheduler;

// Stores all the simulation settings from the command line.
typedef struct s_config
{
	int			number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			number_of_compiles_required;
	long		dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

// Stores one coder's request for a dongle.
typedef struct s_request
{
	int		coder_id;
	long	request_time;
	long	deadline;
}	t_request;

// Stores the waiting coders for one dongle.
typedef struct s_heap
{
	t_request	*items;
	int			size;
	int			capacity;
	t_scheduler	scheduler;
}	t_heap;

// Stores everything related to one dongle.
typedef struct s_dongle
{
	int				id;
	int				in_use;
	long			available_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	condition;
	t_heap			waiting;
}	t_dongle;

struct s_simulation;

// Stores everything related to one coder.
typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	long				last_compile_start;
	int					compile_count;
	pthread_mutex_t		state_mutex;
	struct s_simulation	*simulation;
}	t_coder;

// Stores everything shared by the whole simulation.
typedef struct s_simulation
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor_thread;
	long			start_time;
	int				stopped;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
}	t_simulation;

#endif