/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: redline <redline@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:34:42 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/14 18:31:25 by redline          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	required_compiles(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_coders)
	{
		if (data->coders[i].compiles_done < data->nb_compiles_req)
			return (false);
		i++;
	}
	return (true);
}

static void	announce_burnout(t_data *data, int coder_id)
{
	long	time_ms;

	time_ms = get_current_time_ms() - data->start_time_ms;
	pthread_mutex_lock(&data->write_lock);
	printf("%ld %d burned out\n", time_ms, coder_id);
	pthread_mutex_unlock(&data->write_lock);
}

static int	check_coders(t_data *data, long now)
{
	int	i;

	i = 0;
	while (i < data->nb_coders)
	{
		if (data->coders[i].compiles_done < data->nb_compiles_req && now
			- data->coders[i].last_compile_start_ms >= data->t_burnout)
			return (data->coders[i].id);
		i++;
	}
	if (required_compiles(data) == true)
		data->sim_active = false;
	return (-1);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		burned_id;

	data = (t_data *)arg;
	while (check_sim_active(data) == true)
	{
		pthread_mutex_lock(&data->state_lock);
		burned_id = check_coders(data, get_current_time_ms());
		if (burned_id != -1)
			data->sim_active = false;
		pthread_mutex_unlock(&data->state_lock);
		if (burned_id != -1)
		{
			announce_burnout(data, burned_id);
			break ;
		}
		if (check_sim_active(data) == false)
			break ;
		usleep(1000);
	}
	return (NULL);
}

void	print_two_statuses(t_coder *coder, char *status)
{
	long	current_time;

	pthread_mutex_lock(&coder->data->state_lock);
	if (coder->data->sim_active == true)
	{
		pthread_mutex_lock(&coder->data->write_lock);
		current_time = get_current_time_ms() - coder->data->start_time_ms;
		printf("%ld %d %s\n", current_time, coder->id, status);
		current_time = get_current_time_ms() - coder->data->start_time_ms;
		printf("%ld %d %s\n", current_time, coder->id, status);
		pthread_mutex_unlock(&coder->data->write_lock);
	}
	pthread_mutex_unlock(&coder->data->state_lock);
}
