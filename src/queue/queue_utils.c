/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 12:52:33 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 12:57:23 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	queue_clear(t_queue *queue)
{
	t_node	*current;

	if (queue == NULL)
		return ;
	while (queue->head != NULL)
	{
		current = queue->head->next;
		free(queue->head);
		queue->head = current;
	}
	queue->size = 0;
}

int	queue_peek(t_queue *queue, t_request *request)
{
	if (queue == NULL || queue->head == NULL)
		return (0);
	*request = queue->head->req;
	return (1);
}
