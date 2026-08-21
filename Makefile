# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jbarreir <jbarreir@student.42madrid.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/17 11:38:13 by jbarreir          #+#    #+#              #
#    Updated: 2026/08/21 11:45:31 by jbarreir         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= codexion
CFLAGS		= -Wall -Wextra -Werror -g -pthread -I includes
CC			= cc
HEADER		= codexion.h

# Sustituir con finales
SRC			= $(wildcard */*.c) $(wildcard */*/*.c)

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