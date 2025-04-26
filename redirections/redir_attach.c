#include "minishell.h"

static void attach_redir_to_cmd(t_tree *cmd_node, t_redirection *redir_list)
{
	if (cmd_node && (cmd_node->type == PIPE || cmd_node->type == AND))
		cmd_node->right->redirects = redir_list;
	else if (cmd_node && cmd_node->type == COMMAND)
		cmd_node->redirects = redir_list;
}

void attach_all_redirections(t_tree *node)
{
	t_redirection *redir_list;
	t_tree *cmd_node;

	if (!node)
		return ;
	redir_list = NULL;
	if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION ||
		node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION)
	{
		cmd_node = extract_redirections(node, &redir_list);
		attach_redir_to_cmd(cmd_node, redir_list);
		attach_all_redirections(node->left);
	}
	else
		attach_all_redirections(node->left);
}
