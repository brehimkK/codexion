#include "codexion.h"

int	is_running(t_simulation *simulation)
{
	int	running;

	pthread_mutex_lock(&simulation->mutex);
	running = simulation->running;
	pthread_mutex_unlock(&simulation->mutex);
	return (running);
}

void	wake_all(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.coders)
	{
		pthread_mutex_lock(&simulation->dongles[i].mutex);
		pthread_cond_broadcast(&simulation->dongles[i].condition);
		pthread_mutex_unlock(&simulation->dongles[i].mutex);
		i++;
	}
}