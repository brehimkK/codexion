/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 13:10:50 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 14:03:31 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_simulation(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.coders)
	{
		queue_clear(&simulation->dongles[i].queue);
		pthread_mutex_destroy(&simulation->coders[i].mutex);
		i++;
	}
	cleanup_dongles(simulation, simulation->config.coders);
	pthread_mutex_destroy(&simulation->finished_mutex);
	pthread_mutex_destroy(&simulation->log_mutex);
	pthread_mutex_destroy(&simulation->mutex);
	pthread_mutex_destroy(&simulation->counter_mutex);
	free(simulation->coders);
	free(simulation->dongles);
}
