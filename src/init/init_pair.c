/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_pair.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 18:00:00 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/10 18:00:00 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_pair_sync(t_simulation *simulation)
{
	if (pthread_mutex_init(&simulation->pair_mutex, NULL) != 0)
		return (0);
	if (pthread_cond_init(&simulation->pair_condition, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->pair_mutex);
		return (0);
	}
	return (1);
}

void	destroy_pair_sync(t_simulation *simulation)
{
	pthread_cond_destroy(&simulation->pair_condition);
	pthread_mutex_destroy(&simulation->pair_mutex);
}

void	destroy_basic_sync(t_simulation *simulation)
{
	pthread_mutex_destroy(&simulation->log_mutex);
	pthread_mutex_destroy(&simulation->mutex);
	pthread_mutex_destroy(&simulation->counter_mutex);
}
