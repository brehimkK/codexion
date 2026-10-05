/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 14:15:33 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 14:15:34 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000L) + (time.tv_usec / 1000L));
}

void	safe_sleep(t_simulation *simulation, long duration)
{
	long	start;

	start = get_time_ms();
	while (is_running(simulation)
		&& (get_time_ms() - start < duration))
		usleep(500);
}
