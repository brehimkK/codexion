/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 14:15:58 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 14:15:59 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cleanup_coders(t_simulation *simulation, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&simulation->coders[i].mutex);
		i++;
	}
}

static void	set_coder(t_simulation *simulation, int i, int n)
{
	simulation->coders[i].id = i + 1;
	simulation->coders[i].compile_count = 0;
	simulation->coders[i].last_compile = 0;
	simulation->coders[i].finished = 0;
	simulation->coders[i].simulation = simulation;
	simulation->coders[i].dongle_a = &simulation->dongles[i];
	simulation->coders[i].dongle_b = &simulation->dongles[(i + 1) % n];
}

int	init_coders(t_simulation *simulation)
{
	int	i;
	int	n;

	i = 0;
	n = simulation->config.coders;
	while (i < n)
	{
		set_coder(simulation, i, n);
		if (pthread_mutex_init(&simulation->coders[i].mutex, NULL) != 0)
		{
			cleanup_coders(simulation, i);
			return (0);
		}
		i++;
	}
	return (1);
}
