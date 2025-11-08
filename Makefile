NAME = taskmaster
CC = c++
CFLAGS = -Wall -Werror -Wextra -Iincl -g3 -std=c++11 
RM = rm -f

SRC = \
	src/main.cpp \
	src/Program.cpp \
	src/ConfigParser.cpp \
	aux/parse_utils.cpp
	

OBJTS = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJTS) $(FT_PRINTF)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJTS) -lm

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
