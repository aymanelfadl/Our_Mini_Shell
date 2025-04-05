CC = cc
CFLAGS = -g -Iincludes -Ilibft

NAME = minishell
HEADER = includes/minishell.h 

LIBFTDIR = libft
LIBFT = $(LIBFTDIR)/libft.a

REDIRECTIONDIR = redirections
PARSINGDIR = parsing
BUILTINSDIR = builtins

REDIRECTIONFILES = $(REDIRECTIONDIR)/append_output_redirection.c $(REDIRECTIONDIR)/output_redirection.c \
                   $(REDIRECTIONDIR)/input_redirection.c $(REDIRECTIONDIR)/herdoc_redirection.c \
                   $(REDIRECTIONDIR)/pipe_redirection.c $(REDIRECTIONDIR)/execution_engine.c \
                   $(REDIRECTIONDIR)/logical_operators.c

PARSING_FILES = $(PARSINGDIR)/parsing.c $(PARSINGDIR)/utils1.c $(PARSINGDIR)/utils2.c \
				$(PARSINGDIR)/mylibft.c $(PARSINGDIR)/extract_path.c $(PARSINGDIR)/tree_ops.c \
				$(PARSINGDIR)/utils3.c

BUILTINS_FILES = $(BUILTINSDIR)/ft_unset.c $(BUILTINSDIR)/builtins_utils.c $(BUILTINSDIR)/builtins_engine.c 



EXECUTION_FILES = $(REDIRECTIONFILES) $(BUILTINS_FILES) main.c 

SRCS = $(PARSING_FILES) $(EXECUTION_FILES)
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(LIBFT) $(HEADER) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lreadline $(LIBFT)

$(LIBFT):
	$(MAKE) -C $(LIBFTDIR)

clean:
	rm -f $(OBJS)
	$(MAKE) -C $(LIBFTDIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFTDIR) fclean

re: fclean all
