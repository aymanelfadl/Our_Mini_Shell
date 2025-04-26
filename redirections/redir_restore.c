#include "minishell.h"

static void restore_one_redir(t_redirection *r)
{
	if (r->origin_fd != -1)
	{
		if (r->type == REDIR_INPUT)
			dup2(r->origin_fd, STDIN_FILENO);
		else if (r->type == REDIR_OUTPUT || r->type == REDIR_APPEND)
			dup2(r->origin_fd, STDOUT_FILENO);
		close(r->origin_fd);
		r->origin_fd = -1;
	}
}

void restore_redirections(t_redirection *rlist)
{
	while (rlist)
	{
		restore_one_redir(rlist);
		rlist = rlist->next;
	}
}
