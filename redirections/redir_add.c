#include "minishell.h"

void add_redirection(t_redir_type type, char *file, t_redirection **list)
{
	t_redirection *redir;
	char *trimmed;

	redir = ft_malloc(sizeof(t_redirection));
	if (!redir)
		return ;
	redir->type = type;
	redir->file = file;
	if (type == REDIR_INPUT || type == REDIR_HEREDOC)
		redir->fd_src = STDIN_FILENO;
	else
		redir->fd_src = STDOUT_FILENO;
	redir->origin_fd = -1;
	redir->heredoc_fd = -1;
	redir->expand_heredoc = 1; // Default to expanding variables in heredocs
	redir->next = NULL;
	redir->next = *list;
	*list = redir;
}
