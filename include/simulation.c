#include "codexion.h"

static void	cleanup_dongles(t_simulation *simulation, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&simulation->dongles[i].mutex);
		pthread_cond_destroy(&simulation->dongles[i].condition);
		destroy_heap(&simulation->dongles[i].waiting);
		i++;
	}
}

static void	cleanup_coders(t_simulation *simulation, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&simulation->coders[i].state_mutex);
		i++;
	}
}

static int	free_simulation_arrays(t_simulation *simulation)
{
	free(simulation->coders);
	free(simulation->dongles);
	simulation->coders = NULL;
	simulation->dongles = NULL;
	return (0);
}

int	init_simulation(t_simulation *simulation, t_config *config)
{
	int	i;
	int	dongles_initialized;
	int	coders_initialized;

	i = 0;
	dongles_initialized = 0;
	coders_initialized = 0;
	simulation->config = *config;
	simulation->stopped = 0;
	simulation->start_time = 0;
	simulation->coders = malloc(sizeof(t_coder)
			* config->number_of_coders);
	simulation->dongles = malloc(sizeof(t_dongle)
			* config->number_of_coders);
	if (!simulation->coders || !simulation->dongles)
		return (free_simulation_arrays(simulation));
	if (pthread_mutex_init(&simulation->stop_mutex, NULL) != 0)
		return (free_simulation_arrays(simulation));
	if (pthread_mutex_init(&simulation->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->stop_mutex);
		return (free_simulation_arrays(simulation));
	}
	while (i < config->number_of_coders)
	{
		simulation->dongles[i].id = i;
		simulation->dongles[i].in_use = 0;
		simulation->dongles[i].available_at = 0;

		if (!heap_init(&simulation->dongles[i].waiting,
				config->number_of_coders, config->scheduler))
			break ;

		if (pthread_mutex_init(&simulation->dongles[i].mutex, NULL) != 0)
		{
			destroy_heap(&simulation->dongles[i].waiting);
			break ;
		}
		if (pthread_cond_init(&simulation->dongles[i].condition, NULL) != 0)
		{
			pthread_mutex_destroy(&simulation->dongles[i].mutex);
			destroy_heap(&simulation->dongles[i].waiting);
			break ;
		}
		dongles_initialized++;

		simulation->coders[i].id = i + 1;
		simulation->coders[i].compile_count = 0;
		simulation->coders[i].last_compile_start = 0;
		simulation->coders[i].simulation = simulation;

		if (pthread_mutex_init(&simulation->coders[i].state_mutex, NULL) != 0)
			break ;

		coders_initialized++;
		simulation->coders[i].left_dongle
			= &simulation->dongles[i];
		simulation->coders[i].right_dongle
			= &simulation->dongles[(i + 1)
			% config->number_of_coders];
		i++;
	}
	if (i != config->number_of_coders)
	{
		cleanup_coders(simulation, coders_initialized);
		cleanup_dongles(simulation, dongles_initialized);
		pthread_mutex_destroy(&simulation->log_mutex);
		pthread_mutex_destroy(&simulation->stop_mutex);
		return (free_simulation_arrays(simulation));
	}
	return (1);
}

static void	*coder_routine(void *arg)
{
	t_coder	*coder;
	long	compile_start;

	coder = (t_coder *)arg;
	if (coder->left_dongle == coder->right_dongle)
	{
		printf("ERROR: there's only one dongle\n");
		stop_simulation(coder->simulation);
		return (NULL);
	}
	while (!is_stopped(coder->simulation))
	{
		if (!take_dongle(coder, coder->left_dongle))
			return (NULL);
		if (!take_dongle(coder, coder->right_dongle))
		{
			release_dongle(coder, coder->left_dongle);
			return (NULL);
		}

		compile_start = get_time_ms();
		if (compile_start == -1)
		{
			release_bouth_dongle(coder);
			stop_simulation(coder->simulation);
			return (NULL);
		}
		coder->last_compile_start = compile_start;
		log_status(coder, "is compiling");
		safe_sleep(coder->simulation->config.time_to_compile,
			coder->simulation);

		release_bouth_dongle(coder);

		coder->compile_count++;
		if (all_coders_finished(coder->simulation))
		{
			stop_simulation(coder->simulation);
			return (NULL);
		}

		log_status(coder, "is debugging");
		safe_sleep(coder->simulation->config.time_to_debug,
			coder->simulation);

		if (is_stopped(coder->simulation))
			break ;

		log_status(coder, "is refactoring");
		safe_sleep(coder->simulation->config.time_to_refactor,
			coder->simulation);
	}
	return (NULL);
}

static int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request	request;
	t_request	*next;
	long		now;

	if (is_stopped(coder->simulation))
		return (0);
	pthread_mutex_lock(&dongle->mutex);
	now = get_time_ms();
	if (now == -1)
	{
		pthread_mutex_unlock(&dongle->mutex);
		stop_simulation(coder->simulation);
		return (0);
	}
	request.coder_id = coder->id;
	request.request_time = now;
	if (coder->last_compile_start != 0)
		request.deadline = coder->last_compile_start
			+ coder->simulation->config.time_to_burnout;
	else
		request.deadline = coder->simulation->start_time
			+ coder->simulation->config.time_to_burnout;
	if (!push_new_request(&dongle->waiting, request))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	// Keep waiting until this coder is allowed to take the dongle.
	while (!is_stopped(coder->simulation))
	{
		next = peek_next_request(&dongle->waiting);
		now = get_time_ms();
		if (now == -1)
		{
			pthread_mutex_unlock(&dongle->mutex);
			stop_simulation(coder->simulation);
			return (0);
		}
		if (next != NULL && next->coder_id == coder->id
			&& !dongle->in_use && now >= dongle->available_at)
		{
			dongle->in_use = 1;
			pop_next_request(&dongle->waiting, &request);
			pthread_mutex_unlock(&dongle->mutex);
			log_status(coder, "has taken a dongle");
			return (1);
		}
		pthread_cond_wait(&dongle->condition, &dongle->mutex);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

static void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	long	now;

	pthread_mutex_lock(&dongle->mutex);

	dongle->in_use = 0;

	now = get_time_ms();
	if (now == -1)
	{
		pthread_mutex_unlock(&dongle->mutex);
		stop_simulation(coder->simulation);
		return ;
	}

	dongle->available_at = now
		+ coder->simulation->config.dongle_cooldown;

	pthread_cond_broadcast(&dongle->condition);

	pthread_mutex_unlock(&dongle->mutex);
}

static void release_bouth_dongle(t_coder * coder)
{
	release_dongle(coder, coder->left_dongle);
	release_dongle(coder, coder->right_dongle);
}

static int	all_coders_finished(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.number_of_coders)
	{
		if (simulation->coders[i].compile_count
			< simulation->config.number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static int	burnout_check(t_coder *coder)
{
	long	time;
	long	deadline;

	pthread_mutex_lock(&coder->state_mutex);

	if (coder->last_compile_start != 0)
		deadline = coder->last_compile_start
			+ coder->simulation->config.time_to_burnout;
	else
		deadline = coder->simulation->start_time
			+ coder->simulation->config.time_to_burnout;

	time = get_time_ms();

	pthread_mutex_unlock(&coder->state_mutex);

	if (time == -1)
	{
		stop_simulation(coder->simulation);
		return (0);
	}
	if (time >= deadline)
		return (1);
	return (0);
}

static void	*monitor_routine(void *arg)
{
	t_simulation	*simulation;
	int				i;

	simulation = (t_simulation *)arg;
	while (!is_stopped(simulation))
	{
		i = 0;
		while (i < simulation->config.number_of_coders)
		{
			if (burnout_check(&simulation->coders[i]))
			{
				log_status(&simulation->coders[i], "burned out");
				stop_simulation(simulation);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
