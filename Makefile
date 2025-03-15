CC = cc

CFLAGS = #-Wall -Wextra -Werror 

NAME = minishell
HEADER = minishell.h

LIBFTDIR = libft
LIBFT = $(LIBFTDIR)/libft.a

PARSING_FILES = parsing.c utils1.c utils2.c mylibft.c extract_path.c tree_ops.c utils3.c 
EXECUTION_FILES = execution_engine.c main.c 

SRCS = $(PARSING_FILES) $(EXECUTION_FILES) 

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(LIBFT) $(HEADER) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lreadline $(LIBFT)
	# valgrind --leak-check=full ./$(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFTDIR)

clean:
	rm -f $(OBJS)
	$(MAKE) -C $(LIBFTDIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFTDIR) fclean

re: fclean all
