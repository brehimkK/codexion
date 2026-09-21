#include "codexion.h"

void	init_queue(t_queue *queue, int (*cmp)(t_request, t_request))
{
	queue->head = NULL;
	queue->size = 0;
	queue->cmp = cmp;
}

int	cmp_fifo(t_request a, t_request b)
{
	return (a.arrival_order < b.arrival_order);
}

int	cmp_edf(t_request a, t_request b)
{
	if (a.deadline != b.deadline)
		return (a.deadline < b.deadline);
	return (a.arrival_order < b.arrival_order);
}

int	queue_push(t_queue *queue, t_request request)
{
	t_node	*new_node;
	t_node	*current;

	new_node = malloc(sizeof(t_node));
	if (new_node == NULL)
		return (0);
	new_node->req = request;
	new_node->next = NULL;
	if (queue->head == NULL
		|| queue->cmp(request, queue->head->req))
	{
		new_node->next = queue->head;
		queue->head = new_node;
		queue->size++;
		return (1);
	}
	current = queue->head;
	while (current->next != NULL
		&& !queue->cmp(request, current->next->req))
		current = current->next;
	new_node->next = current->next;
	current->next = new_node;
	queue->size++;
	return (1);
}

int	queue_pop(t_queue *queue, t_request *request)
{
	t_node	*rm_node;

	if (queue->head == NULL)
		return (0);
	rm_node = queue->head;
	*request = rm_node->req;
	queue->head = rm_node->next;
	queue->size--;
	free(rm_node);
	return (1);
}

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