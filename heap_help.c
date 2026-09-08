/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_help.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:34:32 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/08 16:08:05 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	entry_key(t_pq_entry *e, int scheduler)
{
	if (scheduler == 1)
		return (e->deadline_ms);
	return (e->request_time_ms);
}

bool	has_priority(t_pq_entry *a, t_pq_entry *b, int scheduler)
{
	long	a_key;
	long	b_key;

	a_key = entry_key(a, scheduler);
	b_key = entry_key(b, scheduler);
	if (a_key < b_key)
		return (true);
	if (a_key > b_key)
		return (false);
	if (a->coder->id < b->coder->id)
		return (true);
	return (false);
}

void	pq_swap(t_pq_entry *a, t_pq_entry *b)
{
	t_pq_entry	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	sift_up(t_pqueue *q, int index, int scheduler)
{
	int	next;

	while (index > 0)
	{
		next = (index - 1) / 2;
		if (has_priority(&q->entries[index], &q->entries[next], scheduler)
			== false)
			break ;
		pq_swap(&q->entries[index], &q->entries[next]);
		index = next;
	}
}

void	sift_down(t_pqueue *q, int index, int scheduler)
{
	int	left;
	int	right;
	int	best;

	while ((index * 2) + 1 < q->size)
	{
		left = (index * 2) + 1;
		right = (index * 2) + 2;
		best = index;
		if (has_priority(&q->entries[best], &q->entries[left], scheduler)
			== false)
			best = left;
		if (right < q->size && has_priority(&q->entries[best],
				&q->entries[right], scheduler) == false)
			best = right;
		if (index == best)
			break ;
		pq_swap(&q->entries[best], &q->entries[index]);
		index = best;
	}
}
