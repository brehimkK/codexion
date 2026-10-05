/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 18:35:23 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 14:10:50 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	SCHED_TYPE_FIFO,
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
	pthread_cond_t	condition;
	t_queue			queue;
}	t_dongle;

struct	s_simulation;

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
	int					finished;
}	t_coder;

typedef struct s_simulation
{
	long			request_counter;
	pthread_mutex_t	counter_mutex;
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor;
	long			start_time;
	int				running;
	pthread_mutex_t	mutex;
	pthread_mutex_t	log_mutex;
	int				finished_coders;
	pthread_mutex_t	finished_mutex;
}	t_simulation;

int		init_simulation(t_simulation *simulation, t_config *config);
int		init_dongles(t_simulation *simulation);
int		init_coders(t_simulation *simulation);

int		ft_parse(int ac, char **av, t_config *config);
long	ft_check(char *av);

void	init_queue(t_queue *queue, int (*cmp)(t_request, t_request));
int		cmp_fifo(t_request a, t_request b);
int		cmp_edf(t_request a, t_request b);
int		queue_push(t_queue *queue, t_request request);
int		queue_pop(t_queue *queue, t_request *request);
void	queue_clear(t_queue *queue);
int		queue_peek(t_queue *queue, t_request *request);
int		queue_remove_coder(t_queue *queue, int coder_id);

void	cleanup_dongles(t_simulation *simulation, int count);
long	get_next_order(t_simulation *simulation);

long	get_time_ms(void);
void	safe_sleep(t_simulation *simulation, long duration);

void	*coder_routine(void *arg);
int		coder_is_finished(t_coder *coder);

void	get_dongle_order(t_coder *coder, t_dongle **first,
			t_dongle **second);
int		take_dongles(t_coder *coder);
void	release_dongle(t_simulation *simulation, t_dongle *dongle);
void	release_dongles(t_coder *coder);

int		is_running(t_simulation *simulation);
void	wake_all(t_simulation *simulation);
void	stop_simulation(t_simulation *simulation);

void	*monitore_check(void *arg);

int		check_result(long res, int i);
int		set_numeric_arg(long res, int i, t_config *config);

void	set_coder_last_c(t_simulation *simulation, long time);
int		request_is_first(t_dongle *dongle, int coder_id);
int		cooldown_done(t_dongle *dongle);
t_request	create_request(t_coder *coder);
void	cleanup_simulation(t_simulation *simulation);
void	set_coder_last_c(t_simulation *simulation, long time);

#endif
