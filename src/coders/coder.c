/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 11:02:07 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/08 21:37:49 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	compile_coder(t_coder *coder, long time_to_compile)
{
	if (!take_dongles(coder))
		return (0);
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile = get_time_ms();
	pthread_mutex_unlock(&coder->mutex);
	print_log(coder, " is compiling\n");
	safe_sleep(coder->simulation, time_to_compile);
	if (!is_running(coder->simulation))
	{
		release_dongles(coder);
		return (0);
	}
	pthread_mutex_lock(&coder->mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mutex);
	release_dongles(coder);
	return (1);
}

static int	debug_coder(t_coder *coder, long time_to_debug)
{
	print_log(coder, " is debugging\n");
	safe_sleep(coder->simulation, time_to_debug);
	if (!is_running(coder->simulation))
		return (0);
	return (1);
}

static int	refactor_coder(t_coder *coder, long time_to_refactor)
{
	print_log(coder, " is refactoring\n");
	safe_sleep(coder->simulation, time_to_refactor);
	if (!is_running(coder->simulation))
		return (0);
	return (1);
}

static int	finish_coder(t_coder *coder)
{
	if (!coder_is_finished(coder))
		return (0);
	pthread_mutex_lock(&coder->mutex);
	coder->finished = 1;
	pthread_mutex_unlock(&coder->mutex);
	pthread_mutex_lock(&coder->simulation->finished_mutex);
	coder->simulation->finished_coders++;
	if (coder->simulation->finished_coders
		== coder->simulation->config.coders)
	{
		stop_simulation(coder->simulation);
		wake_all(coder->simulation);
	}
	pthread_mutex_unlock(&coder->simulation->finished_mutex);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	long	time_to_re;
	long	time_to_de;
	long	time_to_co;

	coder = arg;
	time_to_co = coder->simulation->config.time_to_compile;
	time_to_de = coder->simulation->config.time_to_debug;
	time_to_re = coder->simulation->config.time_to_refactor;
	if (coder->id % 2 != 0)
		safe_sleep(coder->simulation,
			coder->simulation->config.time_to_compile
			+ coder->simulation->config.dongle_cooldown);
	while (is_running(coder->simulation))
	{
		if (!compile_coder(coder, time_to_co))
			return (NULL);
		if (!debug_coder(coder, time_to_de))
			return (NULL);
		if (!refactor_coder(coder, time_to_re))
			return (NULL);
		if (finish_coder(coder))
			return (NULL);
	}
	return (NULL);
}
