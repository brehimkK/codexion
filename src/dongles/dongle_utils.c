/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 11:00:00 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 12:32:02 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_is_first(t_dongle *dongle, int coder_id)
{
	t_request	head;

	if (queue_peek(&dongle->queue, &head))
	{
		if (head.coder_id == coder_id)
			return (1);
	}
	return (0);
}

int	cooldown_done(t_dongle *dongle)
{
	if (dongle->available_at == 0)
		return (1);
	return (get_time_ms() >= dongle->available_at);
}
