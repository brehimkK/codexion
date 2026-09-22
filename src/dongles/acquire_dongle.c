#include "codexion.h"
#include <time.h>

static int	request_is_first(t_dongle *dongle, int coder_id)
{
	t_request	top;

	if (!queue_peek(&dongle->queue, &top))
		return (0);
	return (top.coder_id == coder_id);
}

static int	cooldown_done(t_dongle *dongle)
{
	// the dongle has never been used
	if (dongle->available_at == 0)
		return (1);
	return (get_time_ms() >= dongle->available_at);
}

static void	wait_cooldown(t_dongle *dongle)
{
	struct timeval	tv;
	struct timespec	ts;
	long			remaining;
	long			ms;

	remaining = dongle->available_at - get_time_ms();
	if (remaining <= 0)
		return ;
	gettimeofday(&tv, NULL);
	// microseconds into milliseconds
	ms = (tv.tv_usec / 1000) + remaining;
	// 1000 milliseconds into 1 second.
	ts.tv_sec = tv.tv_sec + (ms / 1000);
	ts.tv_nsec = (ms % 1000) * 1000000L;
	pthread_cond_timedwait(&dongle->condition, &dongle->mutex, &ts);
}

static int	acquire_one(t_coder *coder, t_dongle *dongle)
{
	t_request	request;

	request = create_request(coder);
	pthread_mutex_lock(&dongle->mutex);
	if (!queue_push(&dongle->queue, request))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	while (is_running(coder->simulation)
		&& (!request_is_first(dongle, coder->id)
			|| dongle->in_use
			|| !cooldown_done(dongle)))
	{
		if (request_is_first(dongle, coder->id)
			&& !dongle->in_use
			&& !cooldown_done(dongle))
			wait_cooldown(dongle);
		else
			pthread_cond_wait(&dongle->condition, &dongle->mutex);
	}
	if (!is_running(coder->simulation))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	queue_pop(&dongle->queue, &request);
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

int	acquire_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	get_dongle_order(coder, &first, &second);
	if (!acquire_one(coder, first))
		return (0);
	if (!is_running(coder->simulation))
	{
		release_dongle(coder->simulation, first);
		return (0);
	}
	if(first == second)
		return (1);
	if (!acquire_one(coder, second))
	{
		release_dongle(coder->simulation, first);
		return (0);
	}
	if (!is_running(coder->simulation))
	{
		release_dongles(coder);
		return (0);
	}
	return (1);
}