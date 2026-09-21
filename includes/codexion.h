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
	/*first request → first served*/
	SCHED_TYPE_FIFO,
	/*earliest deadline → first served*/
	SCHED_TYPE_EDF
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

/* Parsing */
int	ft_parse(int ac, char **av, t_config *config);
long	ft_check(char *av);

/* Queue */
void	init_queue(t_queue *queue, int (*cmp)(t_request, t_request));
int		cmp_fifo(t_request a, t_request b);
int		cmp_edf(t_request a, t_request b);
int		queue_push(t_queue *queue, t_request request);
int		queue_pop(t_queue *queue, t_request *request);
void	queue_clear(t_queue *queue);
int	queue_peek(t_queue *queue, t_request *request);
/* Cleaning */
void	cleanup_dongles(t_simulation *simulation, int count);

/*request*/
long		get_next_order(t_simulation *simulation);
t_request	create_request(t_coder *coder);

/*utils*/
long	get_time_ms(void);

/* Dongles */
void	get_dongle_order(t_coder *coder, t_dongle **first,
			t_dongle **second);
int		acquire_dongles(t_coder *coder);
void	release_dongle(t_simulation *simulation, t_dongle *dongle);
void	release_dongles(t_coder *coder);

/* Simulation */
int		is_running(t_simulation *simulation);
void	wake_all(t_simulation *simulation);

#endif
