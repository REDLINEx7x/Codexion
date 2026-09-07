/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pqueue.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:34:32 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/07 20:34:10 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool has_priority(t_pq_entry *a, t_pq_entry *b, int scheduler)
{
    long a_key;
    long b_key;

    a_key = entry_key(a, scheduler);
    b_key = entry_key(b, scheduler);
    if (a_key < b_key)
        return true;
    else if (a_key > b_key)
        return false;
    else
    {
        if (a->coder->id < b->coder->id)
        return true;
    else
        return false;
    }
}

static void pq_swap(t_pq_entry *a, t_pq_entry *b)
{
    t_pq_entry temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void    pqueue_push(t_pqueue *q, t_coder *coder, long request_ms, long deadline_ms)
{
    q->entries[q->size].coder = coder;
    q->entries[q->size].request_time_ms = request_ms;
    q->entries[q->size].deadline_ms = deadline_ms;
    q->size++;
}

static long entry_key(t_pq_entry *e, int scheduler)
{
    if(scheduler == 1)
        return (e->deadline_ms);
    return (e->request_time_ms);
}

void    pqueue_remove(t_pqueue *q, t_coder *coder)
{
    int i;

    i = 0;
    while (i < q->size && q->entries[i].coder != coder)
        i++;
    if (i == q->size)
        return ;
    q->entries[i] = q->entries[q->size - 1];
    q->size--;
}

