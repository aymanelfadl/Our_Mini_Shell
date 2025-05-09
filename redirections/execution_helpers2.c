/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_helpers2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:29:06 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:29:06 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	command_path_is_dir(char *path)
{
	ft_putstr_fd("minishell : ", 2);
	ft_putstr_fd(path, 2);
	ft_putstr_fd(": Is a directory\n", 2);
	return (126);
}

int	handle_no_path(t_tree *node)
{
	fprintf(stderr, "minishell: %s: command not found\n", node->args[0]);
	*get_exit_status() = 127;
	return (127);
}

static int	try_apply_redir(t_tree *node)
{
	if (!node)
		return (0);
	if (node->redirects)
		if (apply_redirections(node->redirects))
			return (1);
	return (0);
}

int	execute_command(t_tree *node)
{
	int	status;

	status = 0;
	if (!node)
		return (status);
	if (try_apply_redir(node))
		return (1);
	status = fork_and_exec(node);
	return (status);
}

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
}
