/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 12:57:37 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/10 17:35:00 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_next_order(t_simulation *simulation)
{
	long	order;

	pthread_mutex_lock(&simulation->counter_mutex);
	order = simulation->request_counter;
	simulation->request_counter++;
	pthread_mutex_unlock(&simulation->counter_mutex);
	return (order);
}
