/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zaka.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brel-bou <brel-bou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/26 21:45:07 by zahrabar          #+#    #+#             */
/*   Updated: 2026/10/08 21:52:09 by brel-bou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_strlen(char *msg)
{
	int	i;

	i = 0;
	while (msg[i])
		i++;
	return (i);
}

void	ft_putnbr_fd(int n, int fd)
{
	if (n >= 10)
		ft_putnbr_fd(n / 10, fd);
	write(fd, &(char){(n % 10) + '0'}, 1);
}

void	ft_putnbr_ul(unsigned long n)
{
	if (n >= 10)
		ft_putnbr_ul(n / 10);
	write(1, &(char){(n % 10) + '0'}, 1);
}

void	print_log(t_coder *coder, char *msg)
{
	unsigned long	time;

	pthread_mutex_lock(&coder->simulation->log_mutex);
	time = get_time_ms() - coder->simulation->start_time;
	ft_putnbr_ul(time);
	write(1, " ", 1);
	ft_putnbr_fd(coder->id + 1, 1);
	write(1, msg, ft_strlen(msg));
	pthread_mutex_unlock(&coder->simulation->log_mutex);
}