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

static char *unquote_delimiter(char *str)
{
	char *result;
	int i, j;
	int len;
	int has_quotes = 0;
	
	if (!str)
		return (NULL);
	
	len = ft_strlen(str);
	
	// Simple cases: fully quoted string
	if ((str[0] == '\'' && str[len-1] == '\'') || 
		(str[0] == '\"' && str[len-1] == '\"'))
	{
		return ft_substr(str, 1, len - 2);
	}
	
	// Check if there are any quotes in the string
	for (i = 0; i < len; i++)
	{
		if (str[i] == '\'' || str[i] == '\"')
		{
			has_quotes = 1;
			break;
		}
	}
	
	// No quotes, return a copy of the original string
	if (!has_quotes)
		return ft_strdup(str);
	
	// Handle string with quotes in the middle or mixed quotes
	result = ft_malloc(len + 1);
	if (!result)
		return (NULL);
	
	j = 0;
	for (i = 0; i < len; i++)
	{
		if (str[i] != '\'' && str[i] != '\"')
			result[j++] = str[i];
	}
	result[j] = '\0';
	
	return result;
}

t_tree *extract_redirections(t_tree *node, t_redirection **redir_list)
{
	t_redir_type rtype;
	char *original_arg;
	char *trimmed_arg;
	char *unquoted_delimiter;
	int is_quoted;
	int i;

	if (!node)
		return (NULL);
	if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION ||
		node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION)
	{
		if (node->right && node->right->args)
		{
			rtype = determine_redir_type(node->type);
			original_arg = node->right->args[0];
			trimmed_arg = ft_strtrim(original_arg, " \t\n");
			
			// For heredocs, handle delimiter quoting
			if (rtype == REDIR_HEREDOC)
			{
				// Check if original argument has quotes anywhere
				is_quoted = 0;
				i = 0;
				while (trimmed_arg[i])
				{
					if (trimmed_arg[i] == '\'' || trimmed_arg[i] == '\"')
					{
						is_quoted = 1;
						break;
					}
					i++;
				}
				
				// Unquote the delimiter
				unquoted_delimiter = unquote_delimiter(trimmed_arg);
				
				// Add the redirection with unquoted delimiter
				add_redirection(rtype, unquoted_delimiter, redir_list);
				
				// If quoted, disable expansion
				if (is_quoted && *redir_list)
					(*redir_list)->expand_heredoc = 0;
				
				free(trimmed_arg); // Free the trimmed original since we created a new unquoted version
			}
			else
			{
				// For other redirection types, just add as normal
				add_redirection(rtype, trimmed_arg, redir_list);
			}
		}
		return (extract_redirections(node->left, redir_list));
	}
	return (node);
}
