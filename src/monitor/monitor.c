/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 12:49:50 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 12:50:19 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_burned_out(t_simulation *simulation, long i)
{
	long	current;
	long	last_compile;
	long	code_id;
	int		compile_count;

	current = get_time_ms();
	pthread_mutex_lock(&simulation->coders[i].mutex);
	compile_count = simulation->coders[i].compile_count;
	last_compile = simulation->coders[i].last_compile;
	code_id = simulation->coders[i].id;
	pthread_mutex_unlock(&simulation->coders[i].mutex);
	if (compile_count >= simulation->config.compile_required)
		return (0);
	if (current - last_compile >= simulation->config.time_to_burnout)
	{
		pthread_mutex_lock(&simulation->log_mutex);
		printf("%ld %ld burned out\n",
			current - simulation->start_time, code_id);
		pthread_mutex_unlock(&simulation->log_mutex);
		stop_simulation(simulation);
		wake_all(simulation);
		return (1);
	}
	return (0);
}

static int	check_coders(t_simulation *simulation, long coders)
{
	long	i;

	i = 0;
	while (i < coders)
	{
		if (coder_burned_out(simulation, i))
			return (1);
		i++;
	}
	return (0);
}

void	*monitore_check(void *arg)
{
	long			coders;
	t_simulation	*simulation;

	simulation = arg;
	coders = simulation->config.coders;
	while (is_running(simulation))
	{
		if (check_coders(simulation, coders))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
