# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/17 11:38:13 by jbarreir          #+#    #+#              #
#    Updated: 2026/09/23 16:50:20 by jbarreir         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= codexion
CFLAGS		= -D_DEFAULT_SOURCE -std=c89 -Wall -Wextra -Werror -fsanitize=thread -g -pthread -I includes
CC			= cc
HEADER		= codexion.h

SRC			=	src/main.c \
				src/init/init.c \
				src/init/threads.c \
				src/init/memory.c \
				src/init/start_sequence.c \
				src/monitor/monitor.c \
				src/core/safe_locks.c \
				src/core/solo_coder.c \
				src/core/coder_routine.c \
				src/core/dongle.c \
				src/scheduler/scheduler.c \
				src/utils/utils.c \
				src/utils/arg_parser.c \
				src/utils/awareness.c

OBJ			= $(SRC:.c=.o)

GREEN       = \033[0;32m
RED         = \033[0;31m
YELLOW      = \033[0;33m
NC          = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)Codexion compiled successfully!$(NC)"

%.o: %.c $(HEADER)
	@echo "$(YELLOW)Compiling: $<$(NC)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ)
	@echo "$(RED)Objects removed.$(NC)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)Executable removed.$(NC)"

re: fclean all

.PHONY: all clean fclean re