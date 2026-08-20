/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 10:39:13 by moamhouc          #+#    #+#             */
/*   Updated: 2026/08/19 18:47:01 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H


#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>


typedef struct s_dongle {
    int   id;
    bool  taken;
    long  cooldown_until_ms;
} t_dongle;

typedef struct s_coder {
    int             id;
    pthread_t       thread;
    long            last_compile_start_ms;
    int             compiles_done;
    t_dongle        *left_dongle;
    t_dongle        *right_dongle;
    struct s_data   *data;
} t_coder;

typedef struct s_data {
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
} t_data;



#endif
