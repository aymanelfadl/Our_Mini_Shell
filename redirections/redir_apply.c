#include "minishell.h"

static int redir_input(t_redirection *r)
{
	int fd;
	fd = open(r->file, O_RDONLY);
	if (fd != -1)
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
		return (0);
	}
	return (perror("Output file open error"), 1);
}

static int redir_output(t_redirection *r)
{
	int fd;
	fd = open(r->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd != -1)
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
		return (0);
	}
	return (perror("Output file open error"), 1);
}

static int redir_append(t_redirection *r)
{
	int fd;
	fd = open(r->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
	if (fd != -1)
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
		return (0);
	}
	return (perror("Append file open error"), 1);
}

int apply_redirections(t_redirection *rlist)
{
	int ret;
    
	while (rlist)
	{
		if (rlist->type == REDIR_INPUT)
			rlist->origin_fd = dup(STDIN_FILENO);
		else if (rlist->type == REDIR_OUTPUT || rlist->type == REDIR_APPEND)
			rlist->origin_fd = dup(STDOUT_FILENO);
		if (rlist->type == REDIR_INPUT)
			ret = redir_input(rlist);
		else if (rlist->type == REDIR_OUTPUT)
			ret = redir_output(rlist);
		else if (rlist->type == REDIR_APPEND)
			ret = redir_append(rlist);
		else
			ret = 0;
		if (ret)
		{
			*get_exit_status() = 1;
			return (ret);
		}
		rlist = rlist->next;
	}
	return (0);
}
