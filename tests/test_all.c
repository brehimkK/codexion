#include "codexion.h"
#include <limits.h>

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

static int	test_parser_good(void)
{
	char		*av[] = {
		"codexion", "3", "1000", "200", "300",
		"400", "5", "50", "fifo"
	};
	t_config	config;

	if (!ft_parse(9, av, &config))
		return (0);
	if (config.coders != 3
		|| config.time_to_burnout != 1000
		|| config.time_to_compile != 200
		|| config.time_to_debug != 300
		|| config.time_to_refactor != 400
		|| config.compile_required != 5
		|| config.dongle_cooldown != 50
		|| config.scheduler != SCHED_TYPE_FIFO)
		return (0);
	return (1);
}

static int	test_parser_bad(void)
{
	char		*negative[] = {
		"codexion", "-3", "1000", "200", "300",
		"400", "5", "50", "fifo"
	};
	char		*zero[] = {
		"codexion", "0", "1000", "200", "300",
		"400", "5", "50", "fifo"
	};
	char		*text[] = {
		"codexion", "abc", "1000", "200", "300",
		"400", "5", "50", "fifo"
	};
	char		*empty[] = {
		"codexion", "", "1000", "200", "300",
		"400", "5", "50", "fifo"
	};
	char		*too_big[] = {
		"codexion", "2147483648", "1000", "200", "300",
		"400", "5", "50", "fifo"
	};
	char		*wrong_scheduler[] = {
		"codexion", "3", "1000", "200", "300",
		"400", "5", "50", "wrong"
	};
	char		*few_args[] = {
		"codexion", "3", "1000", "200"
	};
	t_config	config;

	if (ft_parse(9, negative, &config))
		return (0);
	if (ft_parse(9, zero, &config))
		return (0);
	if (ft_parse(9, text, &config))
		return (0);
	if (ft_parse(9, empty, &config))
		return (0);
	if (ft_parse(9, too_big, &config))
		return (0);
	if (ft_parse(9, wrong_scheduler, &config))
		return (0);
	if (ft_parse(4, few_args, &config))
		return (0);
	return (1);
}

static int	test_queue_fifo(void)
{
	t_queue		queue;
	t_request	request;
	t_request	out;

	init_queue(&queue, cmp_fifo);

	request.arrival_order = 3;
	queue_push(&queue, request);
	request.arrival_order = 1;
	queue_push(&queue, request);
	request.arrival_order = 2;
	queue_push(&queue, request);

	if (queue.size != 3)
		return (0);
	if (!queue_pop(&queue, &out) || out.arrival_order != 1)
		return (0);
	if (!queue_pop(&queue, &out) || out.arrival_order != 2)
		return (0);
	if (!queue_pop(&queue, &out) || out.arrival_order != 3)
		return (0);
	if (queue_pop(&queue, &out))
		return (0);
	if (queue.size != 0)
		return (0);
	return (1);
}

static int	test_queue_edf(void)
{
	t_queue		queue;
	t_request	request;
	t_request	out;

	init_queue(&queue, cmp_edf);

	request.deadline = 300;
	request.arrival_order = 1;
	queue_push(&queue, request);

	request.deadline = 100;
	request.arrival_order = 2;
	queue_push(&queue, request);;

	request.deadline = 200;
	request.arrival_order = 3;
	queue_push(&queue, request);;

	if (!queue_pop(&queue, &out) || out.deadline != 100)
		return (0);
	if (!queue_pop(&queue, &out) || out.deadline != 200)
		return (0);
	if (!queue_pop(&queue, &out) || out.deadline != 300)
		return (0);

	return (1);
}

static int	test_queue_tie_and_clear(void)
{
	t_queue		queue;
	t_request	request;
	t_request	out;

	init_queue(&queue, cmp_edf);

	request.deadline = 100;
	request.arrival_order = 3;
	queue_push(&queue, request);

	request.deadline = 100;
	request.arrival_order = 1;
	queue_push(&queue, request);;

	request.deadline = 100;
	request.arrival_order = 2;
	queue_push(&queue, request);;

	if (!queue_pop(&queue, &out) || out.arrival_order != 1)
		return (0);
	if (!queue_pop(&queue, &out) || out.arrival_order != 2)
		return (0);
	if (!queue_pop(&queue, &out) || out.arrival_order != 3)
		return (0);

	request.deadline = 50;
	request.arrival_order = 10;
	queue_push(&queue, request);
	request.arrival_order = 11;
	queue_push(&queue, request);;
	queue_clear(&queue);

	if (queue.head != NULL || queue.size != 0)
		return (0);

	queue_clear(NULL);
	return (1);
}

static int	test_init(void)
{
	char			*av[] = {
		"codexion", "3", "1000", "200", "300",
		"400", "5", "50", "edf"
	};
	t_config		config;
	t_simulation	simulation;

	if (!ft_parse(9, av, &config))
		return (0);
	if (!init_simulation(&simulation, &config))
		return (0);
	if (simulation.config.coders != 3)
		return (0);
	if (simulation.coders[0].id != 1
		|| simulation.coders[1].id != 2
		|| simulation.coders[2].id != 3)
		return (0);
	if (simulation.dongles[0].id != 1
		|| simulation.dongles[1].id != 2
		|| simulation.dongles[2].id != 3)
		return (0);
	if (simulation.coders[0].dongle_a != &simulation.dongles[0]
		|| simulation.coders[0].dongle_b != &simulation.dongles[1])
		return (0);
	if (simulation.coders[1].dongle_a != &simulation.dongles[1]
		|| simulation.coders[1].dongle_b != &simulation.dongles[2])
		return (0);
	if (simulation.coders[2].dongle_a != &simulation.dongles[2]
		|| simulation.coders[2].dongle_b != &simulation.dongles[0])
		return (0);
	cleanup_test(&simulation);
	return (1);
}
static int	test_request(void)
{
	t_config		config;
	t_simulation	simulation;
	t_request		request;

	config.coders = 2;
	config.time_to_burnout = 1000;
	config.time_to_compile = 200;
	config.time_to_debug = 300;
	config.time_to_refactor = 400;
	config.compile_required = 5;
	config.dongle_cooldown = 50;
	config.scheduler = SCHED_TYPE_FIFO;

	if (!init_simulation(&simulation, &config))
		return (0);

	simulation.start_time = 1000;
	request = create_request(&simulation.coders[0]);
	printf("\n--- REQUEST TEST ---\n");
	printf("First request:\n");
	printf("  coder_id      = %d\n", request.coder_id);
	printf("  arrival_order = %ld\n", request.arrival_order);
	printf("  arrival_time  = %ld\n", request.arrival_time);
	printf("  deadline      = %ld\n", request.deadline);

	if (request.coder_id != 1
		|| request.arrival_order != 0
		|| request.deadline != 2000
		|| request.arrival_time <= 0)
	{
		cleanup_dongles(&simulation, simulation.config.coders);
		pthread_mutex_destroy(&simulation.coders[0].mutex);
		pthread_mutex_destroy(&simulation.coders[1].mutex);
		free(simulation.coders);
		free(simulation.dongles);
		pthread_mutex_destroy(&simulation.log_mutex);
		pthread_mutex_destroy(&simulation.mutex);
		pthread_mutex_destroy(&simulation.counter_mutex);
		return (0);
	}
	printf("  first request: PASS\n");

	request = create_request(&simulation.coders[1]);
	printf("\nSecond request:\n");
	printf("  coder_id      = %d\n", request.coder_id);
	printf("  arrival_order = %ld\n", request.arrival_order);

	if (request.coder_id != 2 || request.arrival_order != 1)
	{
		cleanup_dongles(&simulation, simulation.config.coders);
		pthread_mutex_destroy(&simulation.coders[0].mutex);
		pthread_mutex_destroy(&simulation.coders[1].mutex);
		free(simulation.coders);
		free(simulation.dongles);
		pthread_mutex_destroy(&simulation.log_mutex);
		pthread_mutex_destroy(&simulation.mutex);
		pthread_mutex_destroy(&simulation.counter_mutex);
		return (0);
	}
	printf("  second request: PASS\n");

	pthread_mutex_lock(&simulation.coders[0].mutex);
	simulation.coders[0].last_compile = 5000;
	pthread_mutex_unlock(&simulation.coders[0].mutex);

	request = create_request(&simulation.coders[0]);
	printf("\nRequest after previous compile:\n");
	printf("  coder_id      = %d\n", request.coder_id);
	printf("  arrival_order = %ld\n", request.arrival_order);
	printf("  deadline      = %ld\n", request.deadline);

	if (request.arrival_order != 2 || request.deadline != 6000)
	{
		cleanup_dongles(&simulation, simulation.config.coders);
		pthread_mutex_destroy(&simulation.coders[0].mutex);
		pthread_mutex_destroy(&simulation.coders[1].mutex);
		free(simulation.coders);
		free(simulation.dongles);
		pthread_mutex_destroy(&simulation.log_mutex);
		pthread_mutex_destroy(&simulation.mutex);
		pthread_mutex_destroy(&simulation.counter_mutex);
		return (0);
	}
	printf("  later request: PASS\n");

	cleanup_dongles(&simulation, simulation.config.coders);
	pthread_mutex_destroy(&simulation.coders[0].mutex);
	pthread_mutex_destroy(&simulation.coders[1].mutex);
	free(simulation.coders);
	free(simulation.dongles);
	pthread_mutex_destroy(&simulation.log_mutex);
	pthread_mutex_destroy(&simulation.mutex);
	pthread_mutex_destroy(&simulation.counter_mutex);
	return (1);
}
static int	test_ft_check(void)
{
	if (ft_check("123") != 123)
		return (0);
	if (ft_check(" 123 ") != 123)
		return (0);
	if (ft_check("-123") != -2)
		return (0);
	if (ft_check("") != -1)
		return (0);
	if (ft_check("abc") != -1)
		return (0);
	if (ft_check("123abc") != -1)
		return (0);
	if (ft_check("9223372036854775808") != -3)
		return (0);
	if (ft_check("9223372036854775807") != LONG_MAX)
		return (0);
	return (1);
}

int	main(void)
{
	printf("\n=== CODEXION TESTS ===\n\n");

	if (!test_ft_check())
	{
		printf("ft_check: FAIL\n");
		return (1);
	}
	printf("ft_check: OK\n");

	if (!test_parser_good())
	{
		printf("parser valid cases: FAIL\n");
		return (1);
	}
	printf("parser valid cases: OK\n");

	if (!test_parser_bad())
	{
		printf("parser invalid cases: FAIL\n");
		return (1);
	}
	printf("parser invalid cases: OK\n");

	if (!test_queue_fifo())
	{
		printf("queue FIFO: FAIL\n");
		return (1);
	}
	printf("queue FIFO: OK\n");

	if (!test_queue_edf())
	{
		printf("queue EDF: FAIL\n");
		return (1);
	}
	printf("queue EDF: OK\n");

	if (!test_queue_tie_and_clear())
	{
		printf("queue edge cases: FAIL\n");
		return (1);
	}
	printf("queue edge cases: OK\n");

	if (!test_init())
	{
		printf("initialization: FAIL\n");
		return (1);
	}
	if (!test_request())
	{
		printf("request: FAIL\n");
		return (1);
	}
	printf("request: OK\n");
	printf("initialization: OK\n");

	printf("\nALL TESTS PASSED\n");
	return (0);
}