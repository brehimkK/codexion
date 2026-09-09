#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) != 0)
		return (-1);
	return ((time.tv_sec * 1000L) + (time.tv_usec / 1000L));
}

int	is_stopped(t_simulation *simulation)
{
	int	stopped;

	pthread_mutex_lock(&simulation->stop_mutex);
	stopped = simulation->stopped;
	pthread_mutex_unlock(&simulation->stop_mutex);
	return (stopped);
}

void	stop_simulation(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->stop_mutex);
	simulation->stopped = 1;
	pthread_mutex_unlock(&simulation->stop_mutex);
}

void	safe_sleep(long duration, t_simulation *simulation)
{
	long	start;
	long	current;

	start = get_time_ms();
	if (start == -1)
	{
		stop_simulation(simulation);
		return ;
	}
	while (!is_stopped(simulation))
	{
		current = get_time_ms();
		if (current == -1)
		{
			stop_simulation(simulation);
			return ;
		}
		if (current - start >= duration)
			return ;
		usleep(500);
	}
}

// Prints a coder status safely with the elapsed simulation time.
void	log_status(t_coder *coder, const char *status)
{
	t_simulation	*simulation;
	long			timestamp;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->log_mutex);
	if (!is_stopped(simulation))
	{
		timestamp = get_time_ms();
		if (timestamp != -1)
		{
			timestamp -= simulation->start_time;
			printf("%ld %d %s\n", timestamp, coder->id, status);
		}
	}
	pthread_mutex_unlock(&simulation->log_mutex);
}
