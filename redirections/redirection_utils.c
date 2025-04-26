#include "minishell.h"

static void	restore_one_redir(t_redirection *r)
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

void	restore_redirections(t_redirection *rlist)
{
	while (rlist)
	{
		restore_one_redir(rlist);
		rlist = rlist->next;
	}
}

static t_redir_type	determine_redir_type(int node_type)
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

t_tree	*extract_redirections(t_tree *node, t_redirection **redir_list)
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

static void	attach_redir_to_cmd(t_tree *cmd_node, t_redirection *redir_list)
{
	if (cmd_node && (cmd_node->type == PIPE || cmd_node->type == AND))
		cmd_node->right->redirects = redir_list;
	else if (cmd_node && cmd_node->type == COMMAND)
		cmd_node->redirects = redir_list;
}

void	attach_all_redirections(t_tree *node)
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
