#include "minishell.h"

static t_redir_type determine_redir_type(int node_type)
{
	if (node_type == INPUT_REDIRECTION)
		return (REDIR_INPUT);
	else if (node_type == OUTPUT_REDIRECTION)
		return (REDIR_OUTPUT);
	else if (node_type == APP_OUTPUT_REDIRECTION)
		return (REDIR_APPEND);
	else if (node_type == APP_INPUT_REDIRECTION)
		return (REDIR_HEREDOC);
	return (REDIR_NONE);
}

t_tree *extract_redirections(t_tree *node, t_redirection **redir_list)
{
	t_redir_type rtype;

	if (!node)
		return (NULL);
	if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION ||
		node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION)
	{
		if (node->right && node->right->args)
		{
			rtype = determine_redir_type(node->type);
			add_redirection(rtype, ft_strtrim(node->right->args[0], " \t\n"), redir_list);
		}
		return (extract_redirections(node->left, redir_list));
	}
	return (node);
}
