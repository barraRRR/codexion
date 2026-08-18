/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 09:29:04 by jbarreir          #+#    #+#             */
/*   Updated: 2026/08/18 13:07:45 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    *compiling(void *arg)
{
    (void)arg;
    while (1)
    {
        sleep(1);
        printf("Compiling...\n");
    }
    return (NULL);
}

void    *debugging(void *arg)
{
    (void)arg;
    while (1)
    {
        sleep(1);
        printf("Debugging...\n");
    }
    return (NULL);
}

int print_err(char *err, int error_code)
{
    fprintf(stderr, "%s", err);
    return (error_code);
}

int parse_rules(t_monitor *monitor, int argc, char **argv)
{
    int             i;

    if (argc != 9)
        return (print_err(ARG_COUNT_ERR_MSG, ARG_COUNT_ERR));
    i = 1;
    while (i < 8)
        if (!isalldigits(argv[i++]))
            return (print_err(ARG_NUM_ERR_MSG, ARG_NUM_ERR));
    monitor->number_of_coders = atoi(argv[1]);
    monitor->time_to_burnout = atoi(argv[2]);
    monitor->time_to_compile = atoi(argv[3]);
    monitor->time_to_debug = atoi(argv[4]);
    monitor->time_to_refactor = atoi(argv[5]);
    monitor->number_of_compiles_required = atoi(argv[6]);
    monitor->dongle_cooldown = atoi(argv[7]);
    if (strcmp(argv[8], "fifo") == 0)
        monitor->scheduler = FIFO;
    else if (strcmp(argv[8], "edf") == 0)
        monitor->scheduler = EDF;
    else
        return (print_err(SCHED_ERR_MSG, SCHED_ERR));
    return (SUCCESS);
}

void    print_status(t_monitor monitor)
{
    printf("number_of_coders             = %d\n", monitor.number_of_coders);
    printf("time_to_burnout              = %d\n", monitor.time_to_burnout);
    printf("time_to_compile              = %d\n", monitor.time_to_compile);
    printf("time_to_debug                = %d\n", monitor.time_to_debug);
    printf("time_to_refactor             = %d\n", monitor.time_to_refactor);
    printf("number_of_compiles_required  = %d\n", monitor.number_of_compiles_required);
    printf("dongle_cooldown              = %d\n", monitor.dongle_cooldown);
}

int main(int argc, char **argv)
{
    t_monitor          monitor;

    monitor.status = parse_rules(&monitor, argc, argv);
    if (monitor.status != PARSING_COMPLETED)
        return (monitor.status);
    print_status(monitor);
    return (SUCCESS);
}

/*
int main()
{
    t_coder         coder_1;
    t_coder         coder_2;
    t_coder         coder_3;
    
    pthread_create(&coder_1.c_pthread, NULL, compiling, NULL);
    pthread_create(&coder_2.c_pthread, NULL, debugging, NULL);
    pthread_create(&coder_3.c_pthread, NULL, compiling, NULL);

    pthread_join(coder_1.c_pthread, NULL);
    pthread_join(coder_2.c_pthread, NULL);
    pthread_join(coder_3.c_pthread, NULL);
    
    return (0);
}
    */
