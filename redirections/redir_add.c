#include "minishell.h"

int redir_input(t_redirection *r)
{
	int fd;
	fd = open(r->file, O_RDONLY);
	if (fd != -1)
	{
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			close(fd);
			return (perror("dup2 input error"), 1);
		}
		close(fd);
		return (0);
	}
	perror(r->file);
	return (1);
}

int redir_output(t_redirection *r)
{
	int fd;
	fd = open(r->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd != -1)
	{
		if (dup2(fd, STDOUT_FILENO) == -1)
		{
			close(fd);
			return (perror("dup2 output error"), 1);
		}
		close(fd);
		return (0);
	}
	perror(r->file);
	return (1);
}

int redir_append(t_redirection *r)
{
	int fd;
	fd = open(r->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
	if (fd != -1)
	{
		if (dup2(fd, STDOUT_FILENO) == -1)
		{
			close(fd);
			return (perror("dup2 append error"), 1);
		}
		close(fd);
		return (0);
	}
	perror(r->file);
	return (1);
}

int redir_heredoc(t_redirection *r)
{
	if (r->heredoc_fd <= 0)
	{
		ft_putstr_fd("minishell: internal error: heredoc file descriptor missing\n", 2);
		return (1);
	}
	if (dup2(r->heredoc_fd, STDIN_FILENO) == -1)
	{
		perror("dup2 heredoc error");
		return (1);
	}
	close(r->heredoc_fd);
	return (0);
}

void add_redirection(t_redir_type type, char *file, t_redirection **list)
{
	t_redirection *redir;

	redir = ft_malloc(sizeof(t_redirection));
	if (!redir)
		return ;
	redir->type = type;
	redir->file = file;
	if (type == REDIR_INPUT || type == REDIR_HEREDOC)
		redir->fd_src = STDIN_FILENO;
	else
		redir->fd_src = STDOUT_FILENO;
	redir->heredoc_fd = -1;
	redir->expand_heredoc = 1;
	redir->next = *list;
	*list = redir;
}
