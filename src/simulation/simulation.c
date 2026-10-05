/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 12:57:59 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 12:59:36 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_running(t_simulation *simulation)
{
	int	running;

	pthread_mutex_lock(&simulation->mutex);
	running = simulation->running;
	pthread_mutex_unlock(&simulation->mutex);
	return (running);
}

void	wake_all(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.coders)
	{
		pthread_mutex_lock(&simulation->dongles[i].mutex);
		pthread_cond_broadcast(&simulation->dongles[i].condition);
		pthread_mutex_unlock(&simulation->dongles[i].mutex);
		i++;
	}
}

void	stop_simulation(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->mutex);
	simulation->running = 0;
	pthread_mutex_unlock(&simulation->mutex);
}
