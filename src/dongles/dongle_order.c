/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_order.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 12:32:18 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/10 17:35:36 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	get_dongle_order(t_coder *coder, t_dongle **first,
		t_dongle **second)
{
	if (coder->dongle_a->id <= coder->dongle_b->id)
	{
		*first = coder->dongle_a;
		*second = coder->dongle_b;
		return ;
	}
	*first = coder->dongle_b;
	*second = coder->dongle_a;
}

void	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	while (is_running(coder->simulation)
		&& (!request_is_first(dongle, coder->id)
			|| dongle->in_use
			|| !cooldown_done(dongle)))
	{
		if (request_is_first(dongle, coder->id)
			&& !dongle->in_use
			&& !cooldown_done(dongle))
			wait_until_ready(dongle);
		else
			pthread_cond_wait(&dongle->condition, &dongle->mutex);
	}
}
