NAME        = codexion

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread

SRCS        = main.c \
              init.c \
              parsing.c \
              sim.c \
              monitor.c \
              utils.c \
              pqueue.c \
              heap_help.c \
              start.c

OBJS        = $(SRCS:%.c=%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
