/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:33:56 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/11 12:47:32 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	try_acquire(t_coder *coder)
{
	long	now;
	bool	got_it;

	if (coder->left_dongle == coder->right_dongle)
		return (false);
	got_it = false;
	pthread_mutex_lock(&coder->data->state_lock);
	now = get_current_time_ms();
	if (coder->left_dongle->taken == false
		&& now >= coder->left_dongle->cooldown_until_ms
		&& coder->right_dongle->taken == false
		&& now >= coder->right_dongle->cooldown_until_ms)
	{
		if (pqueue_priority(&coder->data->queue, coder, now,
				coder->data->scheduler) == false)
		{
			coder->left_dongle->taken = true;
			coder->right_dongle->taken = true;
			pqueue_pop_min(&coder->data->queue, coder->data->scheduler);
			coder->in_queue = false;
			got_it = true;
		}
	}
	pthread_mutex_unlock(&coder->data->state_lock);
	return (got_it);
}

static bool	acquire_dongles(t_coder *coder)
{
	t_pq_entry	entry;

	pthread_mutex_lock(&coder->data->state_lock);
	if (coder->in_queue == false)
	{
		entry.coder = coder;
		entry.request_time_ms = get_current_time_ms();
		entry.deadline_ms = coder->last_compile_start_ms
			+ coder->data->t_burnout;
		coder->wait_start_ms = entry.request_time_ms;
		pqueue_push(&coder->data->queue, entry, coder->data->scheduler);
		coder->in_queue = true;
	}
	pthread_mutex_unlock(&coder->data->state_lock);
	while (check_sim_active(coder->data) == true)
	{
		if (try_acquire(coder))
			return (true);
		usleep(500);
	}
	leave_queue(coder);
	return (false);
}

static void	release_dongles(t_coder *coder)
{
	long	now;

	pthread_mutex_lock(&coder->data->state_lock);
	now = get_current_time_ms();
	coder->left_dongle->taken = false;
	coder->left_dongle->cooldown_until_ms = now + coder->data->cooldown_ms;
	coder->right_dongle->taken = false;
	coder->right_dongle->cooldown_until_ms = now + coder->data->cooldown_ms;
	pthread_mutex_unlock(&coder->data->state_lock);
}

static void	coder_compile(t_coder *coder)
{
	if (get_compiles_done(coder) >= coder->data->nb_compiles_req)
		return ;
	if (acquire_dongles(coder) == false)
		return ;
	print_status(coder, "has taken a dongle");
	print_status(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->data->state_lock);
	coder->last_compile_start_ms = get_current_time_ms();
	pthread_mutex_unlock(&coder->data->state_lock);
	print_status(coder, "is compiling");
	my_usleep(coder->data->t_compile, coder->data);
	release_dongles(coder);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (check_sim_active(coder->data) == true)
	{
		coder_compile(coder);
		if (check_sim_active(coder->data) == false)
			break ;
		print_status(coder, "is debugging");
		my_usleep(coder->data->t_debug, coder->data);
		print_status(coder, "is refactoring");
		my_usleep(coder->data->t_refactor, coder->data);
		pthread_mutex_lock(&coder->data->state_lock);
		coder->compiles_done++;
		pthread_mutex_unlock(&coder->data->state_lock);
		if (get_compiles_done(coder) >= coder->data->nb_compiles_req)
			break ;
	}
	return (NULL);
}
