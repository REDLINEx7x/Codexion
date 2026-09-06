/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: redline <redline@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 18:34:50 by redline           #+#    #+#             */
/*   Updated: 2026/08/24 19:12:26 by redline          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int main(int ac, char **av)
{
    t_data data;

    if (ac != 9)
    {
        printf("Error: Invalid arguments.\nUsage: ./codexion <nb_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <nb_compiles_req> <cooldown> <fifo/edf>\n");
        return (1);
    }

    if (parse_args(av, &data) != 0)
    {
        return (1); // Exit if the parser found an error
    }


}

