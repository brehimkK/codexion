NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
CPPFLAGS = -Iincludes

SRCS = \
	includes/parsing.c \
	includes/parsing_utils.c\
	src/main.c \
	src/cleanup.c\
	src/coders/coder.c \
	src/coders/coders_compile_cont.c\
	src/coders/time.c\
	src/dongles/acquire_dongle.c \
	src/dongles/dongle_order.c \
	src/dongles/release_dongle.c \
	src/dongles/zaka.c \
	src/dongles/dongle_utils.c\
	src/init/init.c \
	src/init/init_coders.c \
	src/init/init_dongles.c \
	src/monitor/monitor.c \
	src/queue/queue.c \
	src/queue/queue_utils.c\
	src/request/request.c \
	src/simulation/simulation.c \

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
