/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 09:16:41 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/10 14:15:51 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	stop_all_coders(t_data *data, int count)
{
	int	j;

	pthread_mutex_lock(&data->state_lock);
	data->sim_active = false;
	pthread_mutex_unlock(&data->state_lock);
	j = 0;
	while (j < count)
	{
		pthread_join(data->coders[j].thread, NULL);
		j++;
	}
}

int	start_simulation(t_data *data)
{
	int			i;
	pthread_t	monitor_thread;

	data->start_time_ms = get_current_time_ms();
	i = 0;
	while (i < data->nb_coders)
	{
		data->coders[i].last_compile_start_ms = data->start_time_ms;
		if (pthread_create(&data->coders[i].thread, NULL, coder_routine,
				&data->coders[i]) != 0)
		{
			stop_all_coders(data, i);
			return (1);
		}
		i++;
	}
	if (pthread_create(&monitor_thread, NULL, monitor_routine, data) != 0)
	{
		stop_all_coders(data, data->nb_coders);
		return (1);
	}
	while (i-- > 0)
		pthread_join(data->coders[i].thread, NULL);
	pthread_join(monitor_thread, NULL);
	return (0);
}

int	get_compiles_done(t_coder *coder)
{
	int	done;

	pthread_mutex_lock(&coder->data->state_lock);
	done = coder->compiles_done;
	pthread_mutex_unlock(&coder->data->state_lock);
	return (done);
}

// void	leave_queue(t_coder *coder)
//{
//	pthread_mutex_lock(&coder->data->state_lock);
//	if (coder->in_queue == true)
//	{
//		pqueue_remove_coder(&coder->data->queue, coder, coder->data->scheduler);
//		coder->in_queue = false;
//	}
//	pthread_mutex_unlock(&coder->data->state_lock);
//}
