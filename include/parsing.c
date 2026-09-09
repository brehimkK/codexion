#include "codexion.h"
#include <limits.h>

long	ft_check(char *av)
{
	int		j;
	long	value;
	int		digit;

	j = 0;
	value = 0;
	while (av[j] == ' ')
		j++;
	if (av[j] == '\0')
		return (-1);
	if (av[j] == '+' || av[j] == '-')
	{
		if (av[j] == '-')
			return (-2);
		j++;
	}
	if (av[j] == '\0')
		return (-1);
	while (av[j] >= '0' && av[j] <= '9')
	{
		digit = av[j] - '0';
		if (value > (LONG_MAX - digit) / 10)
			return (-3);
		value = (value * 10) + digit;
		j++;
	}
	while (av[j] == ' ')
		j++;
	if (av[j] != '\0')
		return (-1);
	return (value);
}

int	ft_parse(int ac, char **av, t_config *config)
{
	int		i;
	long	res;

	i = 1;
	res = 0;
	if (ac != 9)
	{
		printf("Error: must enter exactly 8 arguments\n");
		return (0);
	}
	while (i < ac - 1)
	{
		res = ft_check(av[i]);
		if (res == -1)
		{
			printf("Error: argument %d must contain valid numbers\n", i);
			return (0);
		}
		if (res == -2)
		{
			printf("Error: argument %d cannot be negative\n", i);
			return (0);
		}
		if (res == -3)
		{
			printf("Error: numeric argument %d is too large\n", i);
			return (0);
		}
		i++;
	}
	res = ft_check(av[1]);
	if (res > INT_MAX)
	{
		printf("Error: number_of_coders is too large\n");
		return (0);
	}
	config->number_of_coders = (int)res;
	if (config->number_of_coders == 0)
	{
		printf("Error: number_of_coders must be greater than 0\n");
		return (0);
	}
	config->time_to_burnout = ft_check(av[2]);
	if (config->time_to_burnout == 0)
	{
		printf("Error: time_to_burnout must be greater than 0\n");
		return (0);
	}
	config->time_to_compile = ft_check(av[3]);
	if (config->time_to_compile == 0)
	{
		printf("Error: time_to_compile must be greater than 0\n");
		return (0);
	}
	config->time_to_debug = ft_check(av[4]);
	if (config->time_to_debug == 0)
	{
		printf("Error: time_to_debug must be greater than 0\n");
		return (0);
	}
	config->time_to_refactor = ft_check(av[5]);
	if (config->time_to_refactor == 0)
	{
		printf("Error: time_to_refactor must be greater than 0\n");
		return (0);
	}
	res = ft_check(av[6]);
	if (res > INT_MAX)
	{
		printf("Error: number_of_compiles_required is too large\n");
		return (0);
	}
	config->number_of_compiles_required = (int)res;
	if (config->number_of_compiles_required == 0)
	{
		printf("Error: number_of_compiles_required must be greater than 0\n");
		return (0);
	}
	config->dongle_cooldown = ft_check(av[7]);
	if (strcmp(av[8], "fifo") == 0)
		config->scheduler = SCHED_POLICY_FIFO;
	else if (strcmp(av[8], "edf") == 0)
		config->scheduler = SCHED_POLICY_EDF;
	else
	{
		printf("Error: scheduler must be exactly 'fifo' or 'edf'\n");
		return (0);
	}
	return (1);
}
