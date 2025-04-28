#include "minishell.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

static int redir_input(t_redirection *r)
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

static int redir_output(t_redirection *r)
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

static int redir_append(t_redirection *r)
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

static int redir_heredoc(t_redirection *r)
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

	return (0);
}

int apply_redirections(t_redirection *rlist)
{
	int ret;
	t_redirection *current = rlist;

	while (current)
	{
		current->origin_fd = -1;

		if (current->type == REDIR_INPUT || current->type == REDIR_HEREDOC)
		{
			current->origin_fd = dup(STDIN_FILENO);
			if (current->origin_fd == -1)
				return (perror("dup stdin error"), 1);
		}
		else if (current->type == REDIR_OUTPUT || current->type == REDIR_APPEND)
		{
			current->origin_fd = dup(STDOUT_FILENO);
			if (current->origin_fd == -1)
				return (perror("dup stdout error"), 1);
		}

		if (current->type == REDIR_INPUT)
			ret = redir_input(current);
		else if (current->type == REDIR_OUTPUT)
			ret = redir_output(current);
		else if (current->type == REDIR_APPEND)
			ret = redir_append(current);
		else if (current->type == REDIR_HEREDOC)
			ret = redir_heredoc(current);
		else
			ret = 0;

		if (ret != 0)
		{
			*get_exit_status() = 1;
			if (current->origin_fd != -1)
				close(current->origin_fd);
			return (ret);
		}
		current = current->next;
	}
	return (0);
}
