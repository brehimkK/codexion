#include "codexion.h"

static void	cleanup_init(t_simulation *simulation)
{
	if (simulation->dongles != NULL)
		free(simulation->dongles);
	if (simulation->coders != NULL)
		free(simulation->coders);
	pthread_mutex_destroy(&simulation->log_mutex);
	pthread_mutex_destroy(&simulation->mutex);
	pthread_mutex_destroy(&simulation->counter_mutex);
	simulation->dongles = NULL;
	simulation->coders = NULL;
}

int	init_simulation(t_simulation *simulation, t_config *config)
{
	simulation->config = *config;
	simulation->coders = NULL;
	simulation->dongles = NULL;
	simulation->monitor = 0;
	simulation->start_time = 0;
	simulation->running = 0;
	simulation->request_counter = 0;

	if (pthread_mutex_init(&simulation->counter_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&simulation->mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->counter_mutex);
		return (0);
	}
	if (pthread_mutex_init(&simulation->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->mutex);
		pthread_mutex_destroy(&simulation->counter_mutex);
		return (0);
	}
	simulation->coders = malloc(sizeof(t_coder)
			* simulation->config.coders);
	if (simulation->coders == NULL)
	{
		pthread_mutex_destroy(&simulation->log_mutex);
		pthread_mutex_destroy(&simulation->mutex);
		pthread_mutex_destroy(&simulation->counter_mutex);
		return (0);
	}
	simulation->dongles = malloc(sizeof(t_dongle)
			* simulation->config.coders);
	if (simulation->dongles == NULL)
	{
		free(simulation->coders);
		simulation->coders = NULL;
		pthread_mutex_destroy(&simulation->log_mutex);
		pthread_mutex_destroy(&simulation->mutex);
		pthread_mutex_destroy(&simulation->counter_mutex);
		return (0);
	}
	if (!init_dongles(simulation))
	{
		cleanup_init(simulation);
		return (0);
	}
	if (!init_coders(simulation))
	{
		cleanup_init(simulation);
		return (0);
	}
	return (1);
}