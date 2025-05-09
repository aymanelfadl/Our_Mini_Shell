#include "minishell.h"

static void	cleanup_node_heredocs(t_tree *node)
{
	t_redirection	*redir;

	if (!node)
		return ;
	if (node->redirects)
	{
		redir = node->redirects;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->heredoc_fd > 0)
			{
				close(redir->heredoc_fd);
				redir->heredoc_fd = -1;
			}
			redir = redir->next;
		}
	}
}

void	cleanup_heredoc_fds(t_tree *node)
{
	if (!node)
		return ;
	cleanup_heredoc_fds(node->left);
	cleanup_heredoc_fds(node->right);
	cleanup_node_heredocs(node);
}

void	handle_eof_warning(const char *delimiter)
{
	ft_putstr_fd("minishell: warning: here-document delimited by ", 2);
	ft_putstr_fd("end-of-file (wanted `", 2);
	ft_putstr_fd((char *)delimiter, 2);
	ft_putstr_fd("')\n", 2);
}