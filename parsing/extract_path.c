/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:01:16 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:01:16 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_path(char *command)
{
	int	i;

	i = 0;
	if (command == NULL)
		return (0);
	while (command[i])
	{
		if (command[i++] == '/')
			return (1);
	}
	return (0);
}

char	*check_paths(char **paths, char **command)
{
	int		i;
	char	*path;
	char	*file;

	i = -1;
	if (!command || !(*command))
		return (NULL);
	if (ft_strcmp(*command, ".") == 0 || ft_strcmp(*command, "..") == 0)
		return (NULL);
	if (**command == 0)
		return (NULL);
	if (is_path(*command))
		return (*command);
	while (paths && paths[++i])
	{
		path = ft_strjoin(paths[i], "/");
		file = ft_strjoin(path, *command);
		if (access(file, X_OK) == 0)
			return (file);
	}
	return (NULL);
}

char	**extract_paths(t_list *envp)
{
	while (envp)
	{
		if (ft_strnstr(envp->content, "PATH=", 5))
			return (ft_split(envp->content + 5, ":"));
		envp = envp->next;
	}
	return (NULL);
}
