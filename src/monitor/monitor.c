#include "codexion.h"

void	*monitore_check(void *arg)
{
	long			i;
	long			current;
	long			coders;
	long			last_compile;
	long			time_to_burnout;
	long			code_id;
	t_simulation	*simulation;

	simulation = arg;
	coders = simulation->config.coders;
	time_to_burnout = simulation->config.time_to_burnout;
	while (is_running(simulation))
	{
		i = 0;
		while (i < coders)
		{
			current = get_time_ms();
			pthread_mutex_lock(&simulation->coders[i].mutex);
			last_compile = simulation->coders[i].last_compile;
			code_id = simulation->coders[i].id;
			pthread_mutex_unlock(&simulation->coders[i].mutex);
			if (current - last_compile >= time_to_burnout)
			{
				pthread_mutex_lock(&simulation->log_mutex);
				printf("%ld %ld burned out\n",
					current - simulation->start_time, code_id);
				pthread_mutex_unlock(&simulation->log_mutex);
				stop_simulation(simulation);
				wake_all(simulation);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}