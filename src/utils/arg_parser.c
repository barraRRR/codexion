/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 13:22:11 by jbarreir          #+#    #+#             */
/*   Updated: 2026/09/16 08:47:23 by jbarreir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	isalldigits(char *str)
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

int	int_validator(char **argv, int *arr)
{
	int					i;

	i = -1;
	while (++i < 7)
	{
		if (!isalldigits(argv[i]))
			return (print_err(ARG_NUM_ERR, ARG_NUM_ERR_MSG, NULL, false));
		arr[i] = atoi(argv[i]);
		if (arr[i] < 0)
			return (print_err(ARG_NUM_ERR, ARG_NUM_ERR_MSG, NULL, false));
	}
	return (SUCCESS);
}

int	parse_rules(t_simulation *sim, int argc, char **argv)
{
	int					validator;
	int					arr[7];

	if (argc != 9)
		return (print_err(ARG_COUNT_ERR, ARG_COUNT_ERR_MSG, sim, false));
	validator = int_validator(argv + 1, arr);
	if (validator != SUCCESS)
		return (validator);
	if (arr[0] < 1 || arr[0] > MAX_CODERS)
		return (print_err(MAX_COD_ERR, MAX_COD_ERR_MSG, sim, false));
	sim->number_of_coders = arr[0];
	sim->time_to_burnout = arr[1];
	sim->time_to_compile = arr[2];
	sim->time_to_debug = arr[3];
	sim->time_to_refactor = arr[4];
	sim->compiles_required = arr[5];
	sim->dongle_cooldown = arr[6];
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = EDF;
	else
		return (print_err(SCHED_ERR, SCHED_ERR_MSG, sim, false));
	return (PARSING_COMPLETED);
}
