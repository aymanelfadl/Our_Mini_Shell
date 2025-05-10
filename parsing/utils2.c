/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:01:16 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:01:16 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	isdirectory(char *path)
{
	struct stat	statbuf;

	if (stat(path, &statbuf) != 0)
		return (0);
	return (S_ISDIR(statbuf.st_mode));
}

char	*get_command(char *all_command)
{
	char	*command_name;
	char	*command_head;

	command_head = all_command;
	while (*all_command)
	{
		if (*all_command == 34 || *all_command == 39)
			all_command = ft_strchr(all_command + 1, *all_command) + 1;
		else if (*all_command == ' ' || *all_command == '\t')
		{
			if (string_is_inside(command_head, all_command
					- command_head) == INSIDE_NOTHING)
			{
				command_name = ft_substr(command_head, 0, all_command
						- command_head);
				return (command_name);
			}
			else
				all_command++;
		}
		else
			all_command++;
	}
	return (ft_strdup(command_head));
}

char	*handle_commandes_quoets(char *tree_data)
{
	char	*command;
	char	*rest;

	rest = NULL;
	command = skip_spaces(tree_data);
	command = get_command(command);
	rest = skip_spaces(tree_data) + ft_strlen(command);
	if (*skip_spaces(rest) == 0)
		rest = NULL;
	command = remove_quotes(command);
	command = ft_strjoin("\x01", command);
	command = ft_strjoin(command, "\x01");
	if (rest)
		return (ft_strjoin(ft_strjoin(command, " "),
				handle_commandes_quoets(rest)));
	return (command);
}

t_list	*strings_to_list(char **strings)
{
	t_list	*head;
	char	*dup_str;

	head = NULL;
	while (strings && *strings)
	{
		dup_str = ft_strdup(*strings);
		ft_lstadd_back(&head, ft_lstnew(dup_str));
		strings++;
	}
	return (head);
}

char	*get_original_eof(char *tree_data)
{
	char	*head;

	head = tree_data;
	if (!ft_strchr(tree_data, ' '))
		return (tree_data);
	while (*tree_data)
	{
		if (*tree_data == ' ' && (string_is_inside(head, tree_data
					- head) == INSIDE_NOTHING))
			return (ft_substr(head, 0, tree_data - head));
		tree_data++;
	}
	return (head);
}
