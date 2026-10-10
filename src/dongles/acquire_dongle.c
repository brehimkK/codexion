/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_dongle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 11:00:00 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/10 17:35:21 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	log_dongle(t_coder *coder)
{
	if (is_running(coder->simulation))
		print_log(coder, " has taken a dongle\n");
}

static t_request	create_request(t_coder *coder)
{
	t_request	request;
	long		last_compile;

	request.coder_id = coder->id;
	request.arrival_time = get_time_ms();
	request.arrival_order = get_next_order(coder->simulation);
	pthread_mutex_lock(&coder->mutex);
	last_compile = coder->last_compile;
	pthread_mutex_unlock(&coder->mutex);
	request.deadline = last_compile
		+ coder->simulation->config.time_to_burnout;
	return (request);
}

static int	acquire_one(t_coder *coder, t_dongle *dongle)
{
	t_request	request;

	request = create_request(coder);
	pthread_mutex_lock(&dongle->mutex);
	if (!queue_push(&dongle->queue, request))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	wait_for_dongle(coder, dongle);
	if (!is_running(coder->simulation))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	queue_pop(&dongle->queue, &request);
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

static int	take_second_dongle(t_coder *coder, t_dongle *first,
		t_dongle *second)
{
	if (!acquire_one(coder, second))
	{
		release_dongle(coder->simulation, first);
		return (0);
	}
	log_dongle(coder);
	if (!is_running(coder->simulation))
	{
		release_dongles(coder);
		return (0);
	}
	return (1);
}

int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	get_dongle_order(coder, &first, &second);
	if (first == second)
		return (0);
	if (!acquire_one(coder, first))
		return (0);
	log_dongle(coder);
	if (!is_running(coder->simulation))
	{
		release_dongle(coder->simulation, first);
		return (0);
	}
	return (take_second_dongle(coder, first, second));
}
