/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pqueue.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:34:32 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/11 10:29:39 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	pqueue_push(t_pqueue *q, t_pq_entry entry, int scheduler)
{
	int	index;

	index = q->size;
	q->entries[index] = entry;
	q->size++;
	sift_up(q, index, scheduler);
}

t_pq_entry	pqueue_pop_min(t_pqueue *q, int scheduler)
{
	t_pq_entry	best_entry;
	t_pq_entry	empty_entry;

	if (q->size == 0)
	{
		empty_entry.coder = NULL;
		empty_entry.request_time_ms = 0;
		empty_entry.deadline_ms = 0;
		return (empty_entry);
	}
	best_entry = q->entries[0];
	q->entries[0] = q->entries[q->size - 1];
	q->size--;
	sift_down(q, 0, scheduler);
	return (best_entry);
}

//bool	pqueue_min_is(t_pqueue *q, t_coder *coder)
//{
//	if (q->size == 0)
//		return (false);
//	return (q->entries[0].coder == coder);
//}

void	pqueue_remove_coder(t_pqueue *q, t_coder *coder, int scheduler)
{
	int	i;
	int	parent;

	i = 0;
	while (i < q->size && q->entries[i].coder != coder)
		i++;
	if (i == q->size)
		return ;
	q->size--;
	if (i == q->size)
		return ;
	q->entries[i] = q->entries[q->size];
	parent = (i - 1) / 2;
	if (i > 0 && has_priority(&q->entries[i], &q->entries[parent], scheduler))
		sift_up(q, i, scheduler);
	else
		sift_down(q, i, scheduler);
}

void	leave_queue(t_coder *coder)
{
	pthread_mutex_lock(&coder->data->state_lock);
	if (coder->in_queue == true)
	{
		pqueue_remove_coder(&coder->data->queue, coder, coder->data->scheduler);
		coder->in_queue = false;
	}
	pthread_mutex_unlock(&coder->data->state_lock);
}


bool pqueue_priority(t_pqueue *q, t_coder *coder, long now, int scheduler)
{
	int i;
	int idx;
	t_coder *next;

	idx = 0;
	while (idx < q->size && q->entries[idx].coder != coder)
		idx++;
	if (idx == q->size)
		return (false);
	i = -1;
	while (++i < q->size)
	{
		if (i == idx || has_priority(&q->entries[i], &q->entries[idx], scheduler) == false)
			continue;
		next = q->entries[i].coder;
		if (next->left_dongle->taken == false
			&& now >= next->left_dongle->cooldown_until_ms
			&& next->right_dongle->taken == false
			&& now >= next->right_dongle->cooldown_until_ms)
			return (true);
	}
	return (false);
}
