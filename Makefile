NAME = taskmaster
CC = gcc
CFLAGS = -Wall -Werror -Wextra -g3 -Iincl
RM = rm -f
LIB = -lftprintf

SRC = \
	aux/auxiliar.c \
	aux/cool_get_next_line.c \
	aux/cool_get_next_line_utils.c \
	aux/ft_atoi.c \
	aux/parse_utils.c \
	src/config.c \
	src/logger.c \
	src/main.c \
	src/parser.c \
	src/process.c \
	src/shell.c \
	src/signal_handler.c

OBJS = $(SRC:.c=.o)

FT_PRINTF_DIR = lib/printf
FT_PRINTF = $(FT_PRINTF_DIR)/libftprintf.a

all: $(NAME)

$(NAME): $(OBJS) $(FT_PRINTF)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -L$(FT_PRINTF_DIR) $(LIB) -lm

$(FT_PRINTF):
	@make -C $(FT_PRINTF_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	@make clean -C $(FT_PRINTF_DIR)

fclean: clean
	$(RM) $(NAME)
	@make fclean -C $(FT_PRINTF_DIR)

re: fclean all

.PHONY: all clean fclean re $(FT_PRINTF)
