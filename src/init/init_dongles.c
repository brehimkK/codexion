#include "codexion.h"

static void	cleanup_dongles(t_simulation *simulation, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_cond_destroy(&simulation->dongles[i].condition);
		pthread_mutex_destroy(&simulation->dongles[i].mutex);
		i++;
	}
}

int	init_dongles(t_simulation *simulation)
{
	int	i;
	int	(*cmp)(t_request, t_request);

	if (simulation->config.scheduler == SCHED_FIFO)
		cmp = cmp_fifo;
	else
		cmp = cmp_edf;
	i = 0;
	while (i < simulation->config.coders)
	{
		simulation->dongles[i].id = i + 1;
		simulation->dongles[i].in_use = 0;
		simulation->dongles[i].available_at = 0;
		init_queue(&simulation->dongles[i].queue, cmp);
		if (pthread_mutex_init(&simulation->dongles[i].mutex, NULL) != 0)
		{
			cleanup_dongles(simulation, i);
			return (0);
		}
		if (pthread_cond_init(&simulation->dongles[i].condition, NULL) != 0)
		{
			pthread_mutex_destroy(&simulation->dongles[i].mutex);
			cleanup_dongles(simulation, i);
			return (0);
		}
		i++;
	}
	return (1);
}