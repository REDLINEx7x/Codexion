/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_help.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: redline <redline@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:34:32 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/14 22:01:08 by redline          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	has_priority(t_pq_entry *a, t_pq_entry *b, int scheduler)
{
	if (scheduler == 0 && a->request_order != b->request_order)
		return (a->request_order < b->request_order);
	if (scheduler == 1 && a->deadline_ms != b->deadline_ms)
		return (a->deadline_ms < b->deadline_ms);
	if (a->request_order != b->request_order)
		return (a->request_order < b->request_order);
	return (a->coder->id < b->coder->id);
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
		if (has_priority(&q->entries[index], &q->entries[next],
				scheduler) == false)
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
		if (has_priority(&q->entries[best], &q->entries[left],
				scheduler) == false)
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
