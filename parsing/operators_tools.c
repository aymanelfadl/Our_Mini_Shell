/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators_tools.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:01:16 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:01:16 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

char	*skip_ops(char *command)
{
	command = skip_spaces(command);
	if (get_data_type(command) == PIPE
		|| get_data_type(command) == INPUT_REDIRECTION
		|| get_data_type(command) == OUTPUT_REDIRECTION)
		command++;
	else if (get_data_type(command) != NON)
		command += 2;
	command = skip_spaces(command);
	return (command);
}

int	find_next_ops(char *command)
{
	int	i;

	i = 0;
	while (command[i])
	{
		if (get_data_type(&command[i]) != NON && commas_ops_check(&command[i]))
			return (i);
		i++;
	}
	return (-1);
}

enum data_type	get_data_type(char *s)
{
	if (!ft_strncmp(s, "||", 2))
		return (UNEXPECTED_TOKEN);
	if (!ft_strncmp(s, "<<<", 3))
		return (UNEXPECTED_TOKEN);
	if (!ft_strncmp(s, "&&", 2))
		return (UNEXPECTED_TOKEN);
	if (!ft_strncmp(s, ">>", 2))
		return (APP_OUTPUT_REDIRECTION);
	if (!ft_strncmp(s, "<<", 2))
		return (APP_INPUT_REDIRECTION);
	if (!ft_strncmp(s, "|", 1))
		return (PIPE);
	if (!ft_strncmp(s, ">", 1))
		return (OUTPUT_REDIRECTION);
	if (!ft_strncmp(s, "<", 1))
		return (INPUT_REDIRECTION);
	if (!ft_strncmp(s, ";", 1))
		return (UNEXPECTED_TOKEN);
	if (!ft_strncmp(s, "\\", 1))
		return (UNEXPECTED_TOKEN);
	if (!ft_strncmp(s, "(", 1))
		return (UNEXPECTED_TOKEN);
	if (!ft_strncmp(s, ")", 1))
		return (UNEXPECTED_TOKEN);
	return (NON);
}

int	ops_size(char *s, char **all_ops)
{
	int	i;

	i = 0;
	while (*s)
	{
		if (extract_ops_helper(s, all_ops) >= 0)
		{
			s += ft_strlen(all_ops[extract_ops_helper(s, all_ops)]);
			i++;
		}
		else
			s++;
	}
	return (i + 1);
}
