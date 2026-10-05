/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 11:00:00 by brel-bou          #+#    #+#             */
/*   Updated: 2026/10/05 12:02:17 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>

int	check_result(long res, int i)
{
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
	return (1);
}

static int	set_int_arg(long res, int i, t_config *config)
{
	if (i == 1)
	{
		if (res > INT_MAX)
		{
			printf("Error: number_of_coders is too large\n");
			return (0);
		}
		config->coders = (int)res;
		if (config->coders == 0)
			return (printf("Error: number_of_coders must be greater than 0\n"), 0);
	}
	else if (i == 6)
	{
		if (res > INT_MAX)
			return (printf("Error: number_of_compiles_required is too large\n"), 0);
		config->compile_required = (int)res;
		if (config->compile_required == 0)
			return (printf("Error: number_of_compiles_required must be greater than 0\n"), 0);
	}
	return (1);
}

static int	check_time_arg(long res, int i)
{
	if (res != 0 || i == 7)
		return (1);
	if (i == 2)
		printf("Error: time_to_burnout must be greater than 0\n");
	else if (i == 3)
		printf("Error: time_to_compile must be greater than 0\n");
	else if (i == 4)
		printf("Error: time_to_debug must be greater than 0\n");
	else if (i == 5)
		printf("Error: time_to_refactor must be greater than 0\n");
	return (0);
}

int	set_numeric_arg(long res, int i, t_config *config)
{
	if ((i == 1 || i == 6) && !set_int_arg(res, i, config))
		return (0);
	if (!check_time_arg(res, i))
		return (0);
	if (i == 2)
		config->time_to_burnout = res;
	else if (i == 3)
		config->time_to_compile = res;
	else if (i == 4)
		config->time_to_debug = res;
	else if (i == 5)
		config->time_to_refactor = res;
	else if (i == 7)
		config->dongle_cooldown = res;
	return (1);
}