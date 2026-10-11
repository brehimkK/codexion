/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_dongle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/11 15:51:59 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/11 15:52:01 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	log_pair(t_coder *coder)
{
	if (is_running(coder->simulation))
	{
		print_log(coder, " has taken a dongle\n");
		print_log(coder, " has taken a dongle\n");
	}
}

int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	int			ok;

	get_dongle_order(coder, &first, &second);
	if (first == second)
		return (0);
	pthread_mutex_lock(&coder->simulation->pair_mutex);
	ok = queue_request_pair(coder, first, second);
	if (ok)
		ok = wait_and_take_pair(coder, first, second);
	pthread_mutex_unlock(&coder->simulation->pair_mutex);
	if (!ok)
		return (0);
	if (!is_running(coder->simulation))
	{
		release_dongles(coder);
		return (0);
	}
	log_pair(coder);
	return (1);
}
