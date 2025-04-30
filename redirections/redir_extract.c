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

static char *remove_all_quotes(char *str, int len)
{
	char *result;
	int i = 0, j = 0;
	
	result = ft_malloc(len + 1);
	if (!result)
		return (NULL);
	while (i < len)
	{
		if (str[i] != '\'' && str[i] != '"')
			result[j++] = str[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}

static int is_heredoc_quoted(char *str)
{
	int i = 0;
	if (!str)
		return (0);
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

static void handle_heredoc_redir(char *trimmed_arg, t_redirection **redir_list)
{
	if (is_heredoc_quoted(trimmed_arg) && *redir_list)
		(*redir_list)->expand_heredoc = 0;
	add_redirection(REDIR_HEREDOC, remove_all_quotes(trimmed_arg, ft_strlen(trimmed_arg)), redir_list);
}

t_tree *extract_redirections(t_tree *node, t_redirection **redir_list)
{
	t_redir_type rtype;
	char *original_arg;
	char *trimmed_arg;
	if (!node)
		return (NULL);
	if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION ||
		node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION)
	{
		if (node->right && node->right->args)
		{
			rtype = determine_redir_type(node->type);
			if (rtype == REDIR_HEREDOC)
				handle_heredoc_redir(ft_strtrim(node->right->args[0], " \t\n"), redir_list);
			else
				add_redirection(rtype, ft_strtrim(node->right->args[0], " \t\n"), redir_list);
		}
		return (extract_redirections(node->left, redir_list));
	}
	return (node);
}
