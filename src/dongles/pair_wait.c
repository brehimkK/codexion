/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pair_wait.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 18:00:00 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/10 18:00:00 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	pair_ready(t_coder *coder, t_dongle *first,
		t_dongle *second)
{
	return (request_is_first(first, coder->id)
		&& request_is_first(second, coder->id)
		&& !first->in_use && !second->in_use
		&& cooldown_done(first) && cooldown_done(second));
}

static long	pair_cooldown_wait(t_coder *coder, t_dongle *first,
		t_dongle *second)
{
	long	ready_at;
	long	now;

	if (!request_is_first(first, coder->id)
		|| !request_is_first(second, coder->id)
		|| first->in_use || second->in_use)
		return (0);
	ready_at = first->available_at;
	if (second->available_at > ready_at)
		ready_at = second->available_at;
	now = get_time_ms();
	if (ready_at > now)
		return (ready_at - now);
	return (1);
}

static void	wait_pair(t_simulation *sim, long remaining)
{
	struct timeval	tv;
	struct timespec	ts;
	long			ms;

	if (remaining <= 0)
	{
		pthread_cond_wait(&sim->pair_condition, &sim->pair_mutex);
		return ;
	}
	gettimeofday(&tv, NULL);
	ms = tv.tv_usec / 1000 + remaining;
	ts.tv_sec = tv.tv_sec + ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000L;
	pthread_cond_timedwait(&sim->pair_condition, &sim->pair_mutex, &ts);
}

static int	try_take_pair(t_coder *coder, t_dongle *first,
		t_dongle *second, long *remaining)
{
	int			ready;
	t_request	request;

	lock_dongle_pair(first, second);
	ready = pair_ready(coder, first, second);
	if (ready)
	{
		queue_pop(&first->queue, &request);
		queue_pop(&second->queue, &request);
		first->in_use = 1;
		second->in_use = 1;
	}
	else
		*remaining = pair_cooldown_wait(coder, first, second);
	unlock_dongle_pair(first, second);
	return (ready);
}

int	wait_and_take_pair(t_coder *coder, t_dongle *first,
		t_dongle *second)
{
	long	remaining;

	while (is_running(coder->simulation))
	{
		remaining = 0;
		if (try_take_pair(coder, first, second, &remaining))
			return (1);
		wait_pair(coder->simulation, remaining);
	}
	lock_dongle_pair(first, second);
	queue_remove_coder(&first->queue, coder->id);
	queue_remove_coder(&second->queue, coder->id);
	unlock_dongle_pair(first, second);
	pthread_cond_broadcast(&coder->simulation->pair_condition);
	return (0);
}
