#include "codexion.h"

int	main(void)
{
	t_simulation	simulation;
	t_config		config;
	pthread_t		thread;

	config.coders = 1;
	config.time_to_burnout = 1000;
	config.time_to_compile = 100;
	config.time_to_debug = 100;
	config.time_to_refactor = 100;
	config.compile_required = 1;
	config.dongle_cooldown = 0;
	config.scheduler = SCHED_TYPE_FIFO;

	if (!init_simulation(&simulation, &config))
		return (1);

	simulation.start_time = get_time_ms();
	simulation.running = 1;

	simulation.coders[0].last_compile = simulation.start_time;

	if (pthread_create(&thread, NULL, coder_routine,
			&simulation.coders[0]) != 0)
		return (1);

	pthread_join(thread, NULL);

	cleanup_dongles(&simulation, simulation.config.coders);

	pthread_mutex_destroy(&simulation.coders[0].mutex);
	pthread_mutex_destroy(&simulation.counter_mutex);
	pthread_mutex_destroy(&simulation.mutex);
	pthread_mutex_destroy(&simulation.log_mutex);

	free(simulation.coders);
	free(simulation.dongles);

	return (0);
}