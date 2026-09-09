/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:33:33 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/09 18:58:12 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long    get_current_time_ms(void)
{
    struct timeval  tv;
    long            time_ms;

    if (gettimeofday(&tv, NULL) == -1)
        return (-1);
    time_ms = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000);
    return (time_ms);
}

long    ft_atoi(const char *str)
{
    long    res;
    int     i;

    res = 0;
    i = 0;
    while (str[i] >= '0' && str[i] <= '9')
    {
        res = (res * 10) + (str[i] - '0');
        i++;
    }
    return (res);
}

bool    check_sim_active(t_data *data)
{
    bool    status;

    pthread_mutex_lock(&data->state_lock);
    status = data->sim_active;
    pthread_mutex_unlock(&data->state_lock);
    return (status);
}

void    print_status(t_coder *coder, char *status)
{
    long    current_time;
    bool    is_active;

    pthread_mutex_lock(&coder->data->state_lock);
    is_active = coder->data->sim_active;
    pthread_mutex_unlock(&coder->data->state_lock);
    if (is_active == true)
    {
        current_time = get_current_time_ms() - coder->data->start_time_ms;
        pthread_mutex_lock(&coder->data->write_lock);
        printf("%ld %d %s\n", current_time, coder->id, status);
        pthread_mutex_unlock(&coder->data->write_lock);
    }
}

void    my_usleep(long time_to_sleep, t_data *data)
{
    long    start_time;

    start_time = get_current_time_ms();
    while ((get_current_time_ms() - start_time) < time_to_sleep)
    {
        if (check_sim_active(data) == false)
            return ;
        usleep(50);
    }
}
void    my_usleep_full(long time_to_sleep)
{
    long    start_time;

    start_time = get_current_time_ms();
    while ((get_current_time_ms() - start_time) < time_to_sleep)
        usleep(50);
}

