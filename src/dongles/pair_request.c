/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pair_request.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 18:00:00 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/10 18:00:00 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	lock_dongle_pair(t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
}

void	unlock_dongle_pair(t_dongle *first, t_dongle *second)
{
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}

static t_request	create_pair_request(t_coder *coder)
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

int	queue_request_pair(t_coder *coder, t_dongle *first,
		t_dongle *second)
{
	t_request	request;
	int			ok;

	request = create_pair_request(coder);
	lock_dongle_pair(first, second);
	ok = queue_push(&first->queue, request);
	if (ok && !queue_push(&second->queue, request))
	{
		queue_remove_coder(&first->queue, coder->id);
		ok = 0;
	}
	unlock_dongle_pair(first, second);
	return (ok);
}
