/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_compile_cont.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 14:08:04 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 14:08:05 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	coder_is_finished(t_coder *coder)
{
	int	count;

	pthread_mutex_lock(&coder->mutex);
	count = coder->compile_count;
	pthread_mutex_unlock(&coder->mutex);
	if (count >= coder->simulation->config.compile_required)
		return (1);
	return (0);
}
