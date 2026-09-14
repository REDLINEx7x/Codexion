/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: redline <redline@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:33:33 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/14 18:04:26 by redline          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_current_time_ms(void)
{
	struct timeval	tv;
	long			time_ms;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	time_ms = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000);
	return (time_ms);
}

long	ft_atoi(const char *str)
{
	long	res;
	int		i;

	res = 0;
	i = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (res > (LONG_MAX - (str[i] - '0')) / 10)
			return (-1);
		res = (res * 10) + (str[i] - '0');
		i++;
	}
	return (res);
}

bool	check_sim_active(t_data *data)
{
	bool	status;

	pthread_mutex_lock(&data->state_lock);
	status = data->sim_active;
	pthread_mutex_unlock(&data->state_lock);
	return (status);
}

void	print_status(t_coder *coder, char *status)
{
	long	current_time;

	pthread_mutex_lock(&coder->data->state_lock);
	if (coder->data->sim_active == true)
	{
		current_time = get_current_time_ms() - coder->data->start_time_ms;
		pthread_mutex_lock(&coder->data->write_lock);
		printf("%ld %d %s\n", current_time, coder->id, status);
		pthread_mutex_unlock(&coder->data->write_lock);
	}
	pthread_mutex_unlock(&coder->data->state_lock);
}

void	my_usleep(long time_to_sleep, t_data *data)
{
	long	start_time;

	start_time = get_current_time_ms();
	while ((get_current_time_ms() - start_time) < time_to_sleep)
	{
		if (check_sim_active(data) == false)
			return ;
		usleep(50);
	}
}
