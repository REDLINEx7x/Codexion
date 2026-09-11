/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:30:48 by redline           #+#    #+#             */
/*   Updated: 2026/09/07 11:34:57 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_global_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->write_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&data->state_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&data->write_lock);
		return (1);
	}
	return (0);
}

static int	allocate_arrays(t_data *data)
{
	data->coders = malloc(sizeof(t_coder) * data->nb_coders);
	if (data->coders == NULL)
		return (1);
	data->dongles = malloc(sizeof(t_dongle) * data->nb_coders);
	if (data->dongles == NULL)
	{
		free(data->coders);
		return (1);
	}
	data->queue.entries = malloc(sizeof(t_pq_entry) * data->nb_coders);
	if (data->queue.entries == NULL)
	{
		free(data->coders);
		free(data->dongles);
		return (1);
	}
	data->queue.size = 0;
	data->queue.capacity = data->nb_coders;
	return (0);
}

static void	init_dongles(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_coders)
	{
		data->dongles[i].id = i;
		data->dongles[i].taken = false;
		data->dongles[i].cooldown_until_ms = 0;
		i++;
	}
}

static void	init_coders(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_coders)
	{
		data->coders[i].id = i + 1;
		data->coders[i].compiles_done = 0;
		data->coders[i].in_queue = false;
		data->coders[i].data = data;
		data->coders[i].left_dongle = &data->dongles[i];
		data->coders[i].right_dongle = &data->dongles[(i + 1)
			% data->nb_coders];
		i++;
	}
}

int	init_simulation(t_data *data)
{
	if (init_global_mutexes(data) != 0)
		return (1);
	if (allocate_arrays(data) != 0)
	{
		pthread_mutex_destroy(&data->write_lock);
		pthread_mutex_destroy(&data->state_lock);
		return (1);
	}
	init_dongles(data);
	init_coders(data);
	data->sim_active = true;
	return (0);
}
