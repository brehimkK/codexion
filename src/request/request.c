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

t_request	create_request(t_coder *coder)
{
	t_request	request;
	long		last_compile;

	request.coder_id = coder->id;
	request.arrival_time = get_time_ms();
	request.arrival_order = get_next_order(coder->simulation);
	pthread_mutex_lock(&coder->mutex);
	last_compile = coder->last_compile;
	pthread_mutex_unlock(&coder->mutex);
	request.deadline = last_compile
		+ coder->simulation->config.time_to_burnout;
	return (request);
}