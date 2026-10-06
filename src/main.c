/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 13:06:38 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 15:46:13 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

static int	start_threads(t_simulation *simulation)
{
	int	i;
	int	created;

	i = 0;
	created = 0;
	while (i < simulation->config.coders)
	{
		if (pthread_create(&simulation->coders[i].thread, NULL,
				coder_routine, &simulation->coders[i]) != 0)
		{
			stop_simulation(simulation);
			wake_all(simulation);
			break ;
		}
		i++;
		created++;
	}
	return (created);
}

static void	join_threads(t_simulation *simulation, int created)
{
	int	i;

	i = 0;
	while (i < created)
	{
		pthread_join(simulation->coders[i].thread, NULL);
		i++;
	}
	wake_all(simulation);
	pthread_join(simulation->monitor, NULL);
}

static int	start_monitor(t_simulation *simulation)
{
	if (pthread_create(&simulation->monitor, NULL,
			monitore_check, simulation) != 0)
	{
		stop_simulation(simulation);
		cleanup_simulation(simulation);
		return (0);
	}
	return (1);
}

int	main(int ac, char **av)
{
	t_simulation	simulation;
	int				created;

	if (!ft_parse(ac, av, &simulation.config))
		return (1);
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
	if (!start_monitor(&simulation))
		return (1);
	created = start_threads(&simulation);
	join_threads(&simulation, created);
	cleanup_simulation(&simulation);
	return (0);
}
