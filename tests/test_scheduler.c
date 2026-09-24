#include "codexion.h"

typedef struct s_test
{
	t_coder			*coder;
	pthread_mutex_t	*order_mutex;
	int				*first_id;
	int				*finished;
	int				result;
}	t_test;

static void	cleanup_test(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->config.coders)
	{
		queue_clear(&simulation->dongles[i].queue);
		pthread_cond_destroy(&simulation->dongles[i].condition);
		pthread_mutex_destroy(&simulation->dongles[i].mutex);
		pthread_mutex_destroy(&simulation->coders[i].mutex);
		i++;
	}
	free(simulation->coders);
	free(simulation->dongles);
	pthread_mutex_destroy(&simulation->log_mutex);
	pthread_mutex_destroy(&simulation->mutex);
	pthread_mutex_destroy(&simulation->counter_mutex);
}

static void	*coder_test(void *arg)
{
	t_test	*test;

	test = (t_test *)arg;
	test->result = take_dongles(test->coder);
	if (test->result)
	{
		pthread_mutex_lock(test->order_mutex);
		if (*test->finished == 0)
			*test->first_id = test->coder->id;
		(*test->finished)++;
		pthread_mutex_unlock(test->order_mutex);
		usleep(10000);
		release_dongles(test->coder);
	}
	return (NULL);
}

static int	run_scheduler_test(t_scheduler scheduler, int expected_first)
{
	t_config		config;
	t_simulation	simulation;
	t_test			test1;
	t_test			test3;
	pthread_mutex_t	order_mutex;
	pthread_t		thread1;
	pthread_t		thread3;
	long			start_time;
	int				first_id;
	int				finished;

	config.coders = 3;
	config.time_to_burnout = 1000;
	config.time_to_compile = 200;
	config.time_to_debug = 300;
	config.time_to_refactor = 400;
	config.compile_required = 5;
	config.dongle_cooldown = 200;
	config.scheduler = scheduler;

	if (!init_simulation(&simulation, &config))
		return (0);

	start_time = get_time_ms();
	simulation.start_time = start_time;

	pthread_mutex_lock(&simulation.mutex);
	simulation.running = 1;
	pthread_mutex_unlock(&simulation.mutex);

	pthread_mutex_lock(&simulation.coders[0].mutex);
	simulation.coders[0].last_compile = start_time;
	pthread_mutex_unlock(&simulation.coders[0].mutex);

	pthread_mutex_lock(&simulation.coders[2].mutex);
	simulation.coders[2].last_compile = start_time;
	if (scheduler == SCHED_TYPE_EDF)
		simulation.coders[2].last_compile = start_time - 500;
	pthread_mutex_unlock(&simulation.coders[2].mutex);

	pthread_mutex_lock(&simulation.dongles[0].mutex);
	simulation.dongles[0].available_at = start_time + 300;
	pthread_mutex_unlock(&simulation.dongles[0].mutex);

	pthread_mutex_init(&order_mutex, NULL);
	first_id = 0;
	finished = 0;

	test1.coder = &simulation.coders[0];
	test1.order_mutex = &order_mutex;
	test1.first_id = &first_id;
	test1.finished = &finished;
	test1.result = 0;

	test3.coder = &simulation.coders[2];
	test3.order_mutex = &order_mutex;
	test3.first_id = &first_id;
	test3.finished = &finished;
	test3.result = 0;

	if (pthread_create(&thread1, NULL, coder_test, &test1) != 0)
	{
		pthread_mutex_destroy(&order_mutex);
		cleanup_test(&simulation);
		return (0);
	}
	usleep(50000);
	if (pthread_create(&thread3, NULL, coder_test, &test3) != 0)
	{
		pthread_mutex_lock(&simulation.mutex);
		simulation.running = 0;
		pthread_mutex_unlock(&simulation.mutex);
		wake_all(&simulation);
		pthread_join(thread1, NULL);
		pthread_mutex_destroy(&order_mutex);
		cleanup_test(&simulation);
		return (0);
	}

	pthread_join(thread1, NULL);
	pthread_join(thread3, NULL);

	pthread_mutex_lock(&simulation.mutex);
	simulation.running = 0;
	pthread_mutex_unlock(&simulation.mutex);

	printf("\nScheduler: ");
	if (scheduler == SCHED_TYPE_FIFO)
		printf("FIFO\n");
	else
		printf("EDF\n");
	printf("Expected first coder: %d\n", expected_first);
	printf("Actual first coder:   %d\n", first_id);

	pthread_mutex_destroy(&order_mutex);
	cleanup_test(&simulation);

	if (test1.result != 1 || test3.result != 1)
		return (0);
	return (first_id == expected_first);
}

int	main(void)
{
	if (!run_scheduler_test(SCHED_TYPE_FIFO, 1))
	{
		printf("FIFO TEST: FAIL\n");
		return (1);
	}
	printf("FIFO TEST: PASS\n");

	if (!run_scheduler_test(SCHED_TYPE_EDF, 3))
	{
		printf("EDF TEST: FAIL\n");
		return (1);
	}
	printf("EDF TEST: PASS\n");

	printf("\nSCHEDULER TESTS PASSED\n");
	return (0);
}