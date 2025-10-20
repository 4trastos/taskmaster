NAME = taskmaster
CC = gcc
CFLAGS = -Wall -Werror -Wextra -g -Iincl
RM = rm -f
LIB = -lftprintf
TEST = build_tests.sh

SRC = src/main.c src/logger.c src/process.c src/real_commands.c src/shell.c src/signal_handler.c aux/auxiliar.c \
		src/prompt_loop.c src/parser.c aux/ft_atoi.c aux/cool_get_next_line.c aux/cool_get_next_line_utils.c \
		src/ft_free.c
OBJS = $(SRC:.c=.o)

FT_PRINTF_DIR = lib/printf
FT_PRINTF = $(FT_PRINTF_DIR)/libftprintf.a
LDFLAGS = -L$(FT_PRINTF_DIR) $(LIB) -lm -lreadline

all: $(NAME)
	@./$(TEST)

$(NAME): $(OBJS) $(FT_PRINTF)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

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