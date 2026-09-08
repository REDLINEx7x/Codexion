/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:33:56 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/08 18:02:48 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


static  bool try_acquire(t_coder *coder)
{
    long now;
    bool got_it;

    got_it = false;
    pthread_mutex_lock(&coder->data->state_lock);
    if(pqueue_min_is(&coder->data->queue, coder))
    {
        now  = get_current_time_ms();
        if (coder->left_dongle->taken == false
            && now >= coder->left_dongle->cooldown_until_ms
            && coder->right_dongle->taken == false
            && now >= coder->right_dongle->cooldown_until_ms)
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
static bool acquire_dongles(t_coder *coder)
{

    pthread_mutex_lock(&coder->data->state_lock);
    if (coder->in_queue == false)
    {
        coder->wait_start_ms = get_current_time_ms();
        pqueue_push(&coder->data->queue, coder, coder->wait_start_ms, coder->last_compile_start_ms + coder->data->t_burnout, coder->data->scheduler);
        coder->in_queue = true;
    }
    pthread_mutex_unlock(&coder->data->state_lock);
    while (check_sim_active(coder->data) == true)
    {
        if (try_acquire(coder))
            return (true);
        usleep(500);
    }
    pthread_mutex_lock(&coder->data->state_lock);
    if (coder->in_queue == true)
    {
        pqueue_remove_coder(&coder->data->queue, coder, coder->data->scheduler);        coder->in_queue = false;
    }
    pthread_mutex_unlock(&coder->data->state_lock);
    return (false);
}

static void release_dongles(t_coder *coder)
{
    long now;
    pthread_mutex_lock(&coder->data->state_lock);
    now = get_current_time_ms();
    coder->left_dongle->taken = false;
    coder->left_dongle->cooldown_until_ms = now + coder->data->cooldown_ms;
    coder->right_dongle->taken = false;
    coder->right_dongle->cooldown_until_ms = now + coder->data->cooldown_ms;
    pthread_mutex_unlock(&coder->data->state_lock);
}

static void coder_compile(t_coder *coder)
{

    if (acquire_dongles(coder) == false)
        return ;
    print_status(coder, "has taken a dongle");
    print_status(coder, "has taken a dongle");
    pthread_mutex_lock(&coder->data->state_lock);
    coder->last_compile_start_ms = get_current_time_ms();
    pthread_mutex_unlock(&coder->data->state_lock);

    print_status(coder, "is compiling");
    my_usleep(coder->data->t_compile);

    pthread_mutex_lock(&coder->data->state_lock);
    coder->compiles_done++;
    pthread_mutex_unlock(&coder->data->state_lock);
    release_dongles(coder);
}


void    *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    if (coder->id % 2 == 0)
        usleep(1000);
    while (check_sim_active(coder->data) == true)
    {
        coder_compile(coder);
        if (check_sim_active(coder->data) == false)
            break ;
        print_status(coder, "is debugging");
        my_usleep(coder->data->t_debug);
        if (check_sim_active(coder->data) == false)
            break ;
        print_status(coder, "is refactoring");
        my_usleep(coder->data->t_refactor);
    }
    return (NULL);
}


int start_simulation(t_data *data)
{
    int i;
    int j;
    pthread_t monitor_thread;

    data->start_time_ms = get_current_time_ms();
    i = 0;
    while (i < data->nb_coders)
    {
        data->coders[i].last_compile_start_ms = data->start_time_ms;

        if (pthread_create(&data->coders[i].thread, NULL, coder_routine, &data->coders[i]) != 0)
        {
            pthread_mutex_lock(&data->state_lock);
            data->sim_active = false;
            pthread_mutex_unlock(&data->state_lock);
            j = 0;
            while (j < i)
            {
                pthread_join(data->coders[j].thread, NULL);
                j++;
            }
            return (1);
        }
        i++;
    }
    if (pthread_create(&monitor_thread, NULL, monitor_routine, data) != 0)
    {
        pthread_mutex_lock(&data->state_lock);
        data->sim_active = false;
        pthread_mutex_unlock(&data->state_lock);
        j = 0;
        while(j < data->nb_coders)
        {
            pthread_join(data->coders[j].thread, NULL);
            j++;
        }
        return 1;
    }
    i = 0;
    while(i < data->nb_coders)
    {
        pthread_join(data->coders[i].thread, NULL);
        i++;
    }
    pthread_join(monitor_thread, NULL);
    return (0);
}
