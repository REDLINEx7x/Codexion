/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 18:34:50 by redline           #+#    #+#             */
/*   Updated: 2026/09/07 11:34:47 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_simulation(t_data *data)
{
	pthread_mutex_destroy(&data->state_lock);
	pthread_mutex_destroy(&data->write_lock);
	free(data->coders);
	free(data->dongles);
	free(data->queue.entries);
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (argc != 9)
	{
		fprintf(stderr, "Error: Invalid arguments.\n");
		return (1);
	}
	if (parse_args(argv, &data) != 0)
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	if (init_simulation(&data) != 0)
	{
		fprintf(stderr, "Error: initialization failed\n");
		return (1);
	}
	if (start_simulation(&data) != 0)
	{
		fprintf(stderr, "Error: simulation failed to start\n");
		cleanup_simulation(&data);
		return (1);
	}
	cleanup_simulation(&data);
	return (0);
}
