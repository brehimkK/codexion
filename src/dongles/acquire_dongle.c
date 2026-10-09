/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_dongle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 11:00:00 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/08 16:38:41 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
// #include <time.h>

static void	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	while (is_running(coder->simulation)
		&& (!request_is_first(dongle, coder->id)
			|| dongle->in_use
			|| !cooldown_done(dongle)))
	{
		if (request_is_first(dongle, coder->id)
			&& !dongle->in_use
			&& !cooldown_done(dongle))
			wait_until_ready(dongle);
		else
			pthread_cond_wait(&dongle->condition, &dongle->mutex);
	}
}

static void	log_dongle(t_coder *coder)
{
	if (is_running(coder->simulation))
		print_log(coder, " has taken a dongle\n");
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
