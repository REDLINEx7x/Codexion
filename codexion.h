/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 10:39:13 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/07 11:37:54 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdlib.h>
# include <sys/time.h>

typedef struct s_data t_data;

typedef struct s_dongle
{
    int     id;
    bool    taken;
    long    cooldown_until_ms;
}   t_dongle;

typedef struct s_coder
{
    int             id;
    pthread_t       thread;
    long            last_compile_start_ms;
    int             compiles_done;
    long            wait_start_ms;
    bool            in_queue;
    t_dongle        *left_dongle;
    t_dongle        *right_dongle;
    t_data          *data;
}   t_coder;

typedef struct s_pq_entry
{
    t_coder *coder;
    long    request_time_ms;
    long    deadline_ms;
}   t_pq_entry;

typedef struct s_pqueue
{
    t_pq_entry  *entries;
    int         size;
    int         capacity;
}   t_pqueue;

struct s_data
{
    int             nb_coders;
    long            t_burnout;
    long            t_compile;
    long            t_debug;
    long            t_refactor;
    long            cooldown_ms;
    int             nb_compiles_req;
    int             scheduler;
    long            start_time_ms;
    bool            sim_active;

    pthread_mutex_t state_lock;
    pthread_mutex_t write_lock;

    t_dongle        *dongles;
    t_coder         *coders;
    t_pqueue        queue;
};

long        get_current_time_ms(void);
long        ft_atoi(const char *str);
bool        check_sim_active(t_data *data);
void        print_status(t_coder *coder, char *status);
void        my_usleep(long time_to_sleep);
int         parse_args(char **av, t_data *data);
int         init_simulation(t_data *data);
void        *coder_routine(void *arg);
int         start_simulation(t_data *data);
void        *monitor_routine(void *arg);

void        pqueue_push(t_pqueue *q, t_coder *coder, long request_ms, long deadline_ms);
bool        pqueue_is_front(t_pqueue *q, t_coder *coder, int scheduler);
void        pqueue_remove(t_pqueue *q, t_coder *coder);
void        cleanup_simulation(t_data *data);
#endif
