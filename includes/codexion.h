#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

typedef enum e_scheduler
{
	SCHED_FIFO,
	SCHED_EDF
}	t_scheduler;

typedef struct s_config
{
	int			coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			compile_required;
	long		dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

typedef struct s_request
{
	int		coder_id;
	long	arrival_time;
	long	deadline;
}	t_request;

typedef struct s_queue
{
	t_request		*requests;
	int				size;
	int				capacity;
	t_scheduler		scheduler;
}	t_queue;

typedef struct s_dongle
{
	int				id;
	int				in_use;
	long			available_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	condition;
	t_queue			queue;
}	t_dongle;

struct s_simulation;

typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	t_dongle			*dongle_a;
	t_dongle			*dongle_b;
	long				last_compile;
	int					compile_count;
	pthread_mutex_t		mutex;
	struct s_simulation	*simulation;
}	t_coder;

typedef struct s_simulation
{
	t_config		config;
	t_coder			*coders;
	t_dongle			*dongles;
	pthread_t		monitor;
	long			start_time;
	int				running;
	pthread_mutex_t	mutex;
	pthread_mutex_t	log_mutex;
}	t_simulation;

#endif
