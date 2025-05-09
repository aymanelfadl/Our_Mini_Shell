/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_commandes2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:01:16 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:01:16 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static int	get_file_len(char *file_and_command, int *len)
{
	int	file_len;

	file_len = *len;
	while (file_and_command[file_len])
	{
		if ((file_and_command[file_len] == ' '
				|| file_and_command[file_len] == '\t')
			&& string_is_inside(file_and_command, file_len) == INSIDE_NOTHING)
			break ;
		file_len++;
	}
	*len = file_len;
	return (file_len);
}

static char	*first_check(char *command, char *file_and_command)
{
	command = skip_spaces(skip_ops(command));
	if (find_next_ops(command) != -1)
	{
		file_and_command = ft_substr(command, 0, find_next_ops(command));
		command += find_next_ops(command);
	}
	else
	{
		file_and_command = command;
		command = NULL;
	}
	return (command);
}
