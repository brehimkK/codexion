/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 18:56:17 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 12:04:12 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>

static int	check_sign(char *av, int *j)
{
	if (av[*j] == '\0')
		return (-1);
	if (av[*j] == '+' || av[*j] == '-')
	{
		if (av[*j] == '-')
			return (-2);
		(*j)++;
	}
	if (av[*j] == '\0')
		return (-1);
	return (0);
}

long	ft_check(char *av)
{
	int		j;
	long	value;
	int		digit;
	int		result;

	j = 0;
	value = 0;
	while (av[j] == ' ')
		j++;
	result = check_sign(av, &j);
	if (result != 0)
		return (result);
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

static int	parse_numeric_args(char **av, t_config *config)
{
	int		i;
	long	res;

	i = 1;
	while (i < 8)
	{
		res = ft_check(av[i]);
		if (!check_result(res, i))
			return (0);
		if (!set_numeric_arg(res, i, config))
			return (0);
		i++;
	}
	return (1);
}

static int	parse_scheduler(char *scheduler, t_config *config)
{
	if (strcmp(scheduler, "fifo") == 0)
		config->scheduler = SCHED_TYPE_FIFO;
	else if (strcmp(scheduler, "edf") == 0)
		config->scheduler = SCHED_TYPE_EDF;
	else
	{
		printf("Error: scheduler must be exactly 'fifo' or 'edf'\n");
		return (0);
	}
	return (1);
}

int	ft_parse(int ac, char **av, t_config *config)
{
	if (ac != 9)
	{
		printf("Error: must enter exactly 8 arguments\n");
		return (0);
	}
	if (!parse_numeric_args(av, config))
		return (0);
	if (!parse_scheduler(av[8], config))
		return (0);
	return (1);
}
