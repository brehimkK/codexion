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
	//first request → first served
	SCHED_FIFO,
	//earliest deadline → first served
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
	long	arrival_order;
	int		coder_id;
	long	arrival_time;
	long	deadline;
}	t_request;

typedef struct s_node
{
	t_request		req;
	struct s_node	*next;
}	t_node;

typedef struct s_queue
{
	t_node	*head;
	int		size;
	int		(*cmp)(t_request, t_request);
}	t_queue;

typedef struct s_dongle
{
	int				id;
	int				in_use;
	long			available_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	condition; //sleep until something changes.
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
	long			request_counter;
	pthread_mutex_t	counter_mutex;
	t_config		config;
	t_coder			*coders;
	t_dongle			*dongles;
	pthread_t		monitor;
	long			start_time;
	int				running;
	pthread_mutex_t	mutex;
	pthread_mutex_t	log_mutex;
}	t_simulation;

/* Initialization */
int	init_simulation(t_simulation *simulation, t_config *config);
int	init_dongles(t_simulation *simulation);
int	init_coders(t_simulation *simulation);

/* queue */
void	init_queue(t_queue *queue, int (*cmp)(t_request, t_request));
int		cmp_fifo(t_request a, t_request b);
int		cmp_edf(t_request a, t_request b);
#endif
