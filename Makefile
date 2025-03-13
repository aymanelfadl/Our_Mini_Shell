CC = cc
CFLAGS = -Wall -Wextra -Werror 

NAME = mini-shell

HEADER = execution.h

LIBFTDIR = libft

SRCS = execution_engine.c main.c

OBJS = $(SRCS:.c=.o)

LIBFT = $(LIBFTDIR)/libft.a

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT) $(GNL) $(MLX) 
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFTDIR) -lft -lreadline -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFTDIR)

clean:
	rm -f $(OBJS)
	$(MAKE) -C $(LIBFTDIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFTDIR) fclean

re: fclean all

.PHONY: all clean fclean re