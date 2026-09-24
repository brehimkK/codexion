#include "codexion.h"

typedef struct s_test
{
	t_coder	*coder;
	int		result;
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

static int	start_simulation_for_test(t_simulation *simulation,
		t_config *config)
{
	long	start_time;
	int		i;

	if (!init_simulation(simulation, config))
		return (0);
	start_time = get_time_ms();
	simulation->start_time = start_time;
	pthread_mutex_lock(&simulation->mutex);
	simulation->running = 1;
	pthread_mutex_unlock(&simulation->mutex);
	i = 0;
	while (i < config->coders)
	{
		pthread_mutex_lock(&simulation->coders[i].mutex);
		simulation->coders[i].last_compile = start_time;
		pthread_mutex_unlock(&simulation->coders[i].mutex);
		i++;
	}
	return (1);
}

static void	*coder_test(void *arg)
{
	t_test	*test;

	test = (t_test *)arg;
	printf("Coder %d trying...\n", test->coder->id);
	test->result = take_dongles(test->coder);
	if (!test->result)
	{
		printf("Coder %d: acquisition failed\n", test->coder->id);
		return (NULL);
	}
	printf("Coder %d: acquired both\n", test->coder->id);
	usleep(100000);
	release_dongles(test->coder);
	printf("Coder %d: released both\n", test->coder->id);
	return (NULL);
}

static int	test_concurrent(t_config *config)
{
	t_simulation	simulation;
	t_test			tests[5];
	pthread_t		threads[5];
	int				i;

	if (config->coders != 5)
		return (0);
	if (!start_simulation_for_test(&simulation, config))
		return (0);
	printf("\n--- 5 CODER TEST ---\n");
	i = 0;
	while (i < 5)
	{
		tests[i].coder = &simulation.coders[i];
		tests[i].result = 0;
		if (pthread_create(&threads[i], NULL,
				coder_test, &tests[i]) != 0)
		{
			pthread_mutex_lock(&simulation.mutex);
			simulation.running = 0;
			pthread_mutex_unlock(&simulation.mutex);
			wake_all(&simulation);
			while (--i >= 0)
				pthread_join(threads[i], NULL);
			cleanup_test(&simulation);
			return (0);
		}
		i++;
	}
	i = 0;
	while (i < 5)
	{
		pthread_join(threads[i], NULL);
		if (tests[i].result != 1)
		{
			cleanup_test(&simulation);
			return (0);
		}
		i++;
	}
	pthread_mutex_lock(&simulation.mutex);
	simulation.running = 0;
	pthread_mutex_unlock(&simulation.mutex);
	printf("5 coder test: PASS\n");
	cleanup_test(&simulation);
	return (1);
}

static int	test_cooldown(t_config *config)
{
	t_simulation	simulation;
	long			start;
	long			elapsed;

	if (config->coders != 2)
		return (0);
	if (!start_simulation_for_test(&simulation, config))
		return (0);
	printf("\n--- COOLDOWN TEST ---\n");
	if (!take_dongles(&simulation.coders[0]))
	{
		cleanup_test(&simulation);
		return (0);
	}
	release_dongles(&simulation.coders[0]);
	start = get_time_ms();
	if (!take_dongles(&simulation.coders[0]))
	{
		cleanup_test(&simulation);
		return (0);
	}
	elapsed = get_time_ms() - start;
	printf("Required cooldown: %ld ms\n",
		config->dongle_cooldown);
	printf("Actual wait: %ld ms\n", elapsed);
	release_dongles(&simulation.coders[0]);
	pthread_mutex_lock(&simulation.mutex);
	simulation.running = 0;
	pthread_mutex_unlock(&simulation.mutex);
	cleanup_test(&simulation);
	if (elapsed < config->dongle_cooldown)
		return (0);
	printf("cooldown test: PASS\n");
	return (1);
}

static void	*waiter_test(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	printf("Coder %d waiting...\n", coder->id);
	if (take_dongles(coder))
	{
		printf("Coder %d acquired after shutdown\n", coder->id);
		release_dongles(coder);
	}
	else
		printf("Coder %d stopped correctly\n", coder->id);
	return (NULL);
}

static int	test_shutdown(t_config *config)
{
	t_simulation	simulation;
	pthread_t		thread;

	if (config->coders != 2)
		return (0);
	if (!start_simulation_for_test(&simulation, config))
		return (0);
	printf("\n--- SHUTDOWN TEST ---\n");
	if (!take_dongles(&simulation.coders[0]))
	{
		cleanup_test(&simulation);
		return (0);
	}
	if (pthread_create(&thread, NULL, waiter_test,
			&simulation.coders[1]) != 0)
	{
		release_dongles(&simulation.coders[0]);
		cleanup_test(&simulation);
		return (0);
	}
	usleep(50000);
	printf("Stopping simulation...\n");
	pthread_mutex_lock(&simulation.mutex);
	simulation.running = 0;
	pthread_mutex_unlock(&simulation.mutex);
	wake_all(&simulation);
	pthread_join(thread, NULL);
	release_dongles(&simulation.coders[0]);
	cleanup_test(&simulation);
	printf("shutdown test: PASS\n");
	return (1);
}

int	main(int ac, char **av)
{
	t_config	config;

	if (!ft_parse(ac, av, &config))
		return (1);

	if (config.coders == 5)
	{
		if (!test_concurrent(&config))
		{
			printf("5 coder test: FAIL\n");
			return (1);
		}
	}
	if (config.coders == 2)
	{
		if (!test_cooldown(&config))
		{
			printf("cooldown test: FAIL\n");
			return (1);
		}
		if (!test_shutdown(&config))
		{
			printf("shutdown test: FAIL\n");
			return (1);
		}
	}
	printf("\nALL MANUAL DONGLE TESTS PASSED\n");
	return (0);
}