#include "codexion.h"


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

bool    pqueue_is_front(t_pqueue *q, t_coder *coder, int scheduler)
{
    int i;
    long best_key;

    i = 1;
    best_key = 0;

    if (q->size == 0)
        return false;

    while( i < q->size)
    {
        if(entry_key(&q->entries[i], scheduler) < entry_key(&q->entries[best_key], scheduler) )
            best_key = i;
        i++;
    }
    return (q->entries[best_key].coder == coder);
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
