CC = cc
CFLAGS = -g -Iincludes -Ilibft

NAME = minishell
HEADER = includes/minishell.h 

LIBFTDIR = libft
LIBFT = $(LIBFTDIR)/libft.a

REDIRECTIONDIR = redirections
PARSINGDIR = parsing
BUILTINSDIR = builtins

REDIRECTIONFILES = \
	$(REDIRECTIONDIR)/execution_main.c \
	$(REDIRECTIONDIR)/execution_andor.c \
	$(REDIRECTIONDIR)/execution_helpers1.c \
	$(REDIRECTIONDIR)/execution_helpers2.c \
	$(REDIRECTIONDIR)/redir_add.c \
	$(REDIRECTIONDIR)/redir_apply.c \
	$(REDIRECTIONDIR)/redir_attach.c \
	$(REDIRECTIONDIR)/redir_extract.c \
	$(REDIRECTIONDIR)/redir_restore.c \
	$(REDIRECTIONDIR)/execute_pipe.c \
	$(REDIRECTIONDIR)/utils.c \
	$(REDIRECTIONDIR)/redir_heredoc.c \
	$(REDIRECTIONDIR)/redir_heredoc_process.c \
	$(REDIRECTIONDIR)/redir_heredoc1.c \
	$(REDIRECTIONDIR)/redir_heredoc3.c

PARSING_FILES = $(PARSINGDIR)/parsing.c  $(PARSINGDIR)/utils1.c $(PARSINGDIR)/utils2.c \
				$(PARSINGDIR)/mylibft.c $(PARSINGDIR)/extract_path.c $(PARSINGDIR)/tree_ops.c \
				$(PARSINGDIR)/expand_dollar_sign.c $(PARSINGDIR)/unexpectedtoken.c $(PARSINGDIR)/utils4.c \
				$(PARSINGDIR)/operators_tools.c $(PARSINGDIR)/operators_tools2.c $(PARSINGDIR)/ft_split_files.c \
				$(PARSINGDIR)/create_tree.c $(PARSINGDIR)/expand_dollar_sign_tools.c

BUILTINS_FILES = $(BUILTINSDIR)/ft_unset.c $(BUILTINSDIR)/builtins_utils.c $(BUILTINSDIR)/builtins_engine.c \
				$(BUILTINSDIR)/ft_exit.c $(BUILTINSDIR)/ft_env.c $(BUILTINSDIR)/ft_cd.c $(BUILTINSDIR)/ft_echo.c \
				$(BUILTINSDIR)/ft_export.c $(BUILTINSDIR)/ft_pwd.c $(BUILTINSDIR)/ft_export_helper.c $(BUILTINSDIR)/ft_export_push_back.c \
				$(BUILTINSDIR)/ft_echo_helpers.c



EXECUTION_FILES = $(REDIRECTIONFILES) $(BUILTINS_FILES) main.c signals/ft_signals.c 

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
