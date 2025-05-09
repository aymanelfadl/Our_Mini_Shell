#include "minishell.h"

static int	apply_single_redirection(t_redirection *current)
{
	int	ret;

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

int	apply_redirections(t_redirection *rlist)
{
	int				ret;
	t_redirection	*current;

	current = rlist;
	while (current)
	{
		ret = apply_single_redirection(current);
		if (ret != 0)
		{
			*get_exit_status() = 1;
			return (ret);
		}
		current = current->next;
	}
	return (0);
}
