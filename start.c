/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 09:16:41 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/09 18:46:13 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

//static void stop_all_coders(t_data *data, int count)
//{
//    int j;

//    pthread_mutex_lock(&data->state_lock);
//    data->sim_active = false;
//    pthread_mutex_unlock(&data->state_lock);
//    j = 0;
//    while (j < count)
//    {
//        pthread_join(data->coders[j].thread, NULL);
//        j++;
//    }
//}

//int start_simulation(t_data *data)
//{
//    int         i;
//    pthread_t   monitor_thread;

//    data->start_time_ms = get_current_time_ms();
//    i = 0;
//    while (i < data->nb_coders)
//    {
//        data->coders[i].last_compile_start_ms = data->start_time_ms;
//        if (pthread_create(&data->coders[i].thread, NULL, coder_routine, &data->coders[i]) != 0)
//        {
//            stop_all_coders(data, i);
//            return (1);
//        }
//        i++;
//    }
//    if (pthread_create(&monitor_thread, NULL, monitor_routine, data) != 0)
//    {
//        stop_all_coders(data, data->nb_coders);
//        return (1);
//    }
//    i = 0;
//    while (i < data->nb_coders)
//        pthread_join(data->coders[i++].thread, NULL);
//    pthread_join(monitor_thread, NULL);
//    return (0);
//}

int get_compiles_done(t_coder *coder)
{
    int done;

    pthread_mutex_lock(&coder->data->state_lock);
    done = coder->compiles_done;
    pthread_mutex_unlock(&coder->data->state_lock);
    return (done);
}

//int start_simulation(t_data *data)
//{
//    int i;
//    int j;
//    pthread_t monitor_thread;

//    data->start_time_ms = get_current_time_ms();
//    i = 0;
//    while (i < data->nb_coders)
//    {
//        data->coders[i].last_compile_start_ms = data->start_time_ms;

//        if (pthread_create(&data->coders[i].thread, NULL, coder_routine, &data->coders[i]) != 0)
//        {
//            pthread_mutex_lock(&data->state_lock);
//            data->sim_active = false;
//            pthread_mutex_unlock(&data->state_lock);
//            j = 0;
//            while (j < data->nb_coders)
//            {
//                pthread_join(data->coders[j].thread, NULL);
//                j++;
//            }
//            return (1);
//        }
//        i++;
//    }
//    if (pthread_create(&monitor_thread, NULL, monitor_routine, data) != 0)
//    {
//        pthread_mutex_lock(&data->state_lock);
//        data->sim_active = false;
//        pthread_mutex_unlock(&data->state_lock);
//        j = 0;
//        while(j < data->nb_coders)
//        {
//            pthread_join(data->coders[j].thread, NULL);
//            j++;
//        }
//        return (1);
//    }
//    i = 0;
//    while(i < data->nb_coders)
//    {
//        pthread_join(data->coders[i].thread, NULL);
//        i++;
//    }
//    pthread_join(monitor_thread, NULL);
//    return (0);
//}

//bool	pqueue_higher_priority_ready(t_pqueue *q, t_coder *coder, long now, int scheduler)
//{
//	int	my_index;
//	int	i;
//	t_coder	*other;

//	my_index = -1;
//	i = 0;
//	while (i < q->size)
//	{
//		if (q->entries[i].coder == coder)
//			my_index = i;
//		i++;
//	}
//	if (my_index == -1)
//		return (false);
//	i = 0;
//	while (i < q->size)
//	{
//		if (i != my_index
//			&& has_priority(&q->entries[i], &q->entries[my_index], scheduler))
//		{
//			other = q->entries[i].coder;
//			if (other->left_dongle->taken == false
//				&& now >= other->left_dongle->cooldown_until_ms
//				&& other->right_dongle->taken == false
//				&& now >= other->right_dongle->cooldown_until_ms)
//				return (true);
//		}
//		i++;
//	}
//	return (false);
//}
