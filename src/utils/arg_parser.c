/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 13:22:11 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/21 12:53:30 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int isalldigits(char *str)
{
    if (!str || *str == '\0')
        return (0);
    while (*str)
    {
        if (*str < '0' || *str > '9')
            return (0);
        str++;
    }
    return (1);
}

int int_validator(char **argv, int *arr)
{
    int             i;

    i = 0;
    while (i < 7)
    {
        if (!isalldigits(argv[i]))
            return (print_err(ARG_NUM_ERR, ARG_NUM_ERR_MSG));
        arr[i] = atoi(argv[i]);
        if (arr[i++] < 1)
            return (print_err(ARG_INT_ERR, ARG_INT_ERR_MSG));
    }
    return (SUCCESS);
}

int parse_rules(t_simulation *sim, int argc, char **argv)
{
    int             validator;
    int             arr[7];

    if (argc != 9)
        return (print_err(ARG_COUNT_ERR, ARG_COUNT_ERR_MSG));
    validator = int_validator(argv+1, arr);
    if (validator != SUCCESS)
        return (validator);
    sim->number_of_coders = arr[0];
    sim->time_to_burnout = arr[1];
    sim->time_to_compile = arr[2];
    sim->time_to_debug = arr[3];
    sim->time_to_refactor = arr[4];
    sim->number_of_compiles_required = arr[5];
    sim->dongle_cooldown = arr[6];
    sim->completed_compiles = 0;
    if (strcmp(argv[8], "fifo") == 0)
        sim->scheduler = FIFO;
    else if (strcmp(argv[8], "edf") == 0)
        sim->scheduler = EDF;
    else
        return (print_err(SCHED_ERR, SCHED_ERR_MSG));
    return (PARSING_COMPLETED);
}

void print_status(t_simulation sim)
{
    printf("number_of_coders             = %d\n", sim.number_of_coders);
    printf("time_to_burnout              = %lld\n", sim.time_to_burnout);
    printf("time_to_compile              = %lld\n", sim.time_to_compile);
    printf("time_to_debug                = %lld\n", sim.time_to_debug);
    printf("time_to_refactor             = %lld\n", sim.time_to_refactor);
    printf("number_of_compiles_required  = %d\n", sim.number_of_compiles_required);
    printf("dongle_cooldown              = %lld\n", sim.dongle_cooldown);
}