/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   release_dongle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 15:52:18 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/11 15:52:19 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	mark_released(t_simulation *sim, t_dongle *dongle,
		long now)
{
	dongle->in_use = 0;
	dongle->available_at = now + sim->config.dongle_cooldown;
}

void	release_dongle(t_simulation *simulation, t_dongle *dongle)
{
	pthread_mutex_lock(&simulation->pair_mutex);
	pthread_mutex_lock(&dongle->mutex);
	mark_released(simulation, dongle, get_time_ms());
	pthread_cond_broadcast(&simulation->pair_condition);
	pthread_cond_broadcast(&dongle->condition);
	pthread_mutex_unlock(&dongle->mutex);
	pthread_mutex_unlock(&simulation->pair_mutex);
}

void	release_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	long		now;

	get_dongle_order(coder, &first, &second);
	pthread_mutex_lock(&coder->simulation->pair_mutex);
	lock_dongle_pair(first, second);
	now = get_time_ms();
	mark_released(coder->simulation, first, now);
	mark_released(coder->simulation, second, now);
	pthread_cond_broadcast(&coder->simulation->pair_condition);
	pthread_cond_broadcast(&first->condition);
	pthread_cond_broadcast(&second->condition);
	unlock_dongle_pair(first, second);
	pthread_mutex_unlock(&coder->simulation->pair_mutex);
}
