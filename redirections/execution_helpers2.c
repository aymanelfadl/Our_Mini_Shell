#include "minishell.h"

static int try_apply_redir(t_tree *node)
{
	if (node->redirects)
		if (apply_redirections(node->redirects))
			return 1;
	return 0;
}

int execute_command(t_tree *node)
{
	int status;

	if (try_apply_redir(node))
		return 1;
	status = fork_and_exec(node);
	if (node->redirects)
		restore_redirections(node->redirects);
	return status;
}
