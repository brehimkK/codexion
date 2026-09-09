#include "codexion.h"

int	heap_init(t_heap *heap, int capacity, t_scheduler scheduler)
{
	if (heap == NULL || capacity <= 0)
		return (0);
	if (scheduler != SCHED_POLICY_FIFO
		&& scheduler != SCHED_POLICY_EDF)
		return (0);
	heap->items = malloc(sizeof(t_request) * capacity);
	if (heap->items == NULL)
		return (0);
	heap->size = 0;
	heap->capacity = capacity;
	heap->scheduler = scheduler;
	return (1);
}

// Returns 1 when first has higher priority than second.
static int	request_has_priority(t_request *first,
		t_request *second, t_scheduler scheduler)
{
	if (scheduler == SCHED_POLICY_FIFO)
	{
		if (first->request_time != second->request_time)
			return (first->request_time < second->request_time);
	}
	else
	{
		if (first->deadline != second->deadline)
			return (first->deadline < second->deadline);
	}
	if (first->request_time != second->request_time)
		return (first->request_time < second->request_time);
	return (first->coder_id < second->coder_id);
}

static void	swap_requests(t_request *first, t_request *second)
{
	t_request	temp;

	temp = *first;
	*first = *second;
	*second = temp;
}

// Moves a newly inserted request upward to its correct position.
static void	move_new_request_to_correct_place(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!request_has_priority(&heap->items[index],
				&heap->items[parent], heap->scheduler))
			break ;
		swap_requests(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
}

int	push_new_request(t_heap *heap, t_request request)
{
	int	index;

	if (heap == NULL || heap->items == NULL)
		return (0);
	if (heap->size >= heap->capacity)
		return (0);
	index = heap->size;
	heap->items[index] = request;
	heap->size++;
	move_new_request_to_correct_place(heap, index);
	return (1);
}

t_request	*peek_next_request(t_heap *heap)
{
	if (heap == NULL || heap->items == NULL)
		return (NULL);
	if (heap->size == 0)
		return (NULL);
	return (&heap->items[0]);
}

// Move the request at the top down until it reaches the correct position.
static void	move_top_request_to_correct_place(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	priority;

	while (1)
	{
		left = (2 * index) + 1;
		right = (2 * index) + 2;
		priority = index;

		if (left < heap->size
			&& request_has_priority(&heap->items[left],
				&heap->items[priority], heap->scheduler))
			priority = left;

		if (right < heap->size
			&& request_has_priority(&heap->items[right],
				&heap->items[priority], heap->scheduler))
			priority = right;

		if (priority == index)
			break ;

		swap_requests(&heap->items[index],
			&heap->items[priority]);
		index = priority;
	}
}

int	pop_next_request(t_heap *heap, t_request *request)
{
	if (!heap || !heap->items || !request || heap->size == 0)
		return (0);
	*request = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		move_top_request_to_correct_place(heap, 0);
	}
	return (1);
}

void	destroy_heap(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}