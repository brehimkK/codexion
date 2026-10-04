#include "codexion.h"

void	set_coder_last_c(t_simulation *simulation, long time)
{
	int	i;

	i = 0;
	while (i < simulation->config.coders)
	{
		pthread_mutex_lock(&simulation->coders[i].mutex);
		simulation->coders[i].last_compile = time;
		pthread_mutex_unlock(&simulation->coders[i].mutex);
		i++;
	}
}

static void	cleanup_simulation(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.coders)
	{
		queue_clear(&simulation->dongles[i].queue);
		i++;
	}
	cleanup_dongles(simulation, simulation->config.coders);
	i = 0;
	while (i < simulation->config.coders)
	{
		pthread_mutex_destroy(&simulation->coders[i].mutex);
		i++;
	}
	pthread_mutex_destroy(&simulation->finished_mutex);
	pthread_mutex_destroy(&simulation->log_mutex);
	pthread_mutex_destroy(&simulation->mutex);
	pthread_mutex_destroy(&simulation->counter_mutex);
	free(simulation->coders);
	free(simulation->dongles);
}

// void check_leaks()
// {
// 	system("leaks codexion");
// }

int	main(int ac, char **av)
{
	t_simulation	simulation;
	int				i;
	int				created;

	// atexit(check_leaks);
	if (!ft_parse(ac, av, &simulation.config))
		return (1);
	// if (simulation.config.coders == 1)
	// {
	// 	fprintf(stderr, "one coder one table not enough to compile\n");
	// 	return (1);
	// }
	if (!init_simulation(&simulation, &simulation.config))
		return (1);
	simulation.finished_coders = 0;
	if (pthread_mutex_init(&simulation.finished_mutex, NULL) != 0)
	{
		cleanup_simulation(&simulation);
		return (1);
	}
	simulation.start_time = get_time_ms();
	set_coder_last_c(&simulation, simulation.start_time);
	simulation.running = 1;
	if (pthread_create(&simulation.monitor, NULL,
			monitore_check, &simulation) != 0)
	{
		stop_simulation(&simulation);
		cleanup_simulation(&simulation);
		return (1);
	}
	i = 0;
	created = 0;
	while (i < simulation.config.coders)
	{
		if (pthread_create(&simulation.coders[i].thread, NULL,
				coder_routine, &simulation.coders[i]) != 0)
		{
			stop_simulation(&simulation);
			wake_all(&simulation);
			break ;
		}
		i++;
		created++;
	}
	i = 0;
	while (i < created)
	{
		pthread_join(simulation.coders[i].thread, NULL);
		i++;
	}
	wake_all(&simulation);
	pthread_join(simulation.monitor, NULL);
	cleanup_simulation(&simulation);
	return (0);
}