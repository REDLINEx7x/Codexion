/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moamhouc <moamhouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 10:39:13 by moamhouc          #+#    #+#             */
/*   Updated: 2026/09/12 17:53:59 by moamhouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_data	t_data;

typedef struct s_dongle
{
	int					id;
	bool				taken;
	long				cooldown_until_ms;
}						t_dongle;

typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	long				last_compile_start_ms;
	int					compiles_done;
	long				wait_start_ms;
	bool				in_queue;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	t_data				*data;
}						t_coder;

typedef struct s_pq_entry
{
	t_coder				*coder;
	long				request_time_ms;
	long				deadline_ms;
}						t_pq_entry;

typedef struct s_pqueue
{
	t_pq_entry			*entries;
	int					size;
	int					capacity;
}						t_pqueue;

struct					s_data
{
	int					nb_coders;
	long				t_burnout;
	long				t_compile;
	long				t_debug;
	long				t_refactor;
	long				cooldown_ms;
	int					nb_compiles_req;
	int					scheduler;
	long				start_time_ms;
	bool				sim_active;
	pthread_mutex_t		state_lock;
	pthread_mutex_t		write_lock;
	t_dongle			*dongles;
	t_coder				*coders;
	t_pqueue			queue;
};

long					get_current_time_ms(void);
long					ft_atoi(const char *str);
bool					check_sim_active(t_data *data);
void					print_status(t_coder *coder, char *status);
void					my_usleep(long time_to_sleep, t_data *data);
int						parse_args(char **av, t_data *data);
int						init_simulation(t_data *data);
void					*coder_routine(void *arg);
int						start_simulation(t_data *data);
void					*monitor_routine(void *arg);
long					entry_key(t_pq_entry *e, int scheduler);
bool					has_priority(t_pq_entry *a, t_pq_entry *b,
							int scheduler);
void					pq_swap(t_pq_entry *a, t_pq_entry *b);
void					sift_up(t_pqueue *q, int index, int scheduler);
void					sift_down(t_pqueue *q, int index, int scheduler);
void					pqueue_push(t_pqueue *q, t_pq_entry entry,
							int scheduler);
t_pq_entry				pqueue_pop_min(t_pqueue *q, int scheduler);
//bool					pqueue_min_is(t_pqueue *q, t_coder *coder);
bool					pqueue_priority(t_pqueue *q, t_coder *coder, long now,
							int scheduler);
bool					pqueue_is_empty(t_pqueue *q);
void					pqueue_remove_coder(t_pqueue *q, t_coder *coder,
							int scheduler);
void					cleanup_simulation(t_data *data);
int						get_compiles_done(t_coder *coder);
void					leave_queue(t_coder *coder);
#endif
