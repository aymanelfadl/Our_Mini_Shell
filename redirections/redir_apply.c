#include "minishell.h"

static int	save_original_fd(t_redirection *current)
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
	return (0);
}

static int	apply_single_redirection(t_redirection *current)
{
	int ret;

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

	return (ret);
}

int apply_redirections(t_redirection *rlist)
{
	int ret;
	t_redirection *current;

	current = rlist;
	while (current)
	{
		if (save_original_fd(current) != 0)
			return (1);

		ret = apply_single_redirection(current);
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
