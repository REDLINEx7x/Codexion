/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: redline <redline@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 10:39:06 by moamhouc          #+#    #+#             */
/*   Updated: 2026/08/24 20:22:51 by redline          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


static int  is_valid_number(char *str)
{
    int i;

    i = 0;
    if (str[i] == '\0')
        return (1);
    while (str[i] != '\0')
    {
        if (str[i] < '0' || str[i] > '9')
            return (1);
        i++;
    }
    return (0);
}

// Helper to save lines in parse_args
static void fill_data(t_data *data, char **av)
{
    data->nb_coders = ft_atoi(av[1]);
    data->t_burnout = ft_atoi(av[2]);
    data->t_compile = ft_atoi(av[3]);
    data->t_debug = ft_atoi(av[4]);
    data->t_refactor = ft_atoi(av[5]);
    data->nb_compiles_req = ft_atoi(av[6]);
    data->cooldown_ms = ft_atoi(av[7]);
}

// Now exactly 19 lines - easily passes Norminette!
int parse_args(char **av, t_data *data)
{
    int i;

    i = 1;
    while (i <= 7)
    {
        if (is_valid_number(av[i]) != 0)
            return (1);
        i++;
    }
    fill_data(data, av);
    if (data->nb_coders <= 0)
        return (1);
    if (strcmp(av[8], "fifo") == 0)
        data->scheduler = 0;
    else if (strcmp(av[8], "edf") == 0)
        data->scheduler = 1;
    else
        return (1);
    return (0);
}

