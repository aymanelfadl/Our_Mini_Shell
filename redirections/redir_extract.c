/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_extract.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:29:06 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:29:06 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static char	*remove_all_quotes(char *str, int len)
{
	char	*result;
	int		i;
	int		j;
	i = 0;
	j = 0;
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

static int	is_heredoc_quoted(char *str)
{
	int	i;

	i = 0;
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

static void	handle_heredoc_redir(char *trimmed_arg, t_redirection **redir_list)
{
	int		quoted;
	char	*clean_delim;

	quoted = is_heredoc_quoted(trimmed_arg);
	clean_delim = remove_all_quotes(trimmed_arg, ft_strlen(trimmed_arg));
	add_redirection(REDIR_HEREDOC, clean_delim, redir_list);
	if (quoted)
		(*redir_list)->expand_heredoc = 0;
	else
		(*redir_list)->expand_heredoc = 1;
}

t_tree	*extract_redirections(t_tree *node, t_redirection **redir_list)
{
	t_redir_type	rtype;

	if (!node)
		return (NULL);
	if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION
		|| node->type == APP_OUTPUT_REDIRECTION
		|| node->type == APP_INPUT_REDIRECTION)
	{
		if (node->right && node->right->args[0])
		{
			rtype = determine_redir_type(node->type);
			if (rtype == REDIR_HEREDOC)
				handle_heredoc_redir(node->right->args[0], redir_list);
			else
				add_redirection(rtype, node->right->args[0], redir_list);
		}
		return (extract_redirections(node->left, redir_list));
	}
	return (node);
}
