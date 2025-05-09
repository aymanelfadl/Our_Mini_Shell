/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_envp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:29:06 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:29:06 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**get_envp(char **envp)
{
	static char	**saved_envp;

	if (envp)
		saved_envp = envp;
	return (saved_envp);
}

t_list	*initialize_env_list(char **envp)
{
	static t_list	*head;

	if (envp != NULL)
		head = strings_to_list(envp);
	return (head);
}

t_list	*create_minimal_env(void)
{
	char	*minimal_env[4];
	t_list	*env_list;
	char	*cwd;

	env_list = NULL;
	cwd = getcwd(NULL, 0);
	if (!cwd)
		cwd = ft_strdup("/");
	minimal_env[0] = ft_strjoin("PWD=", cwd);
	minimal_env[1] = ft_strdup("SHLVL=1");
	minimal_env[2] = ft_strdup("_=/usr/bin/env");
	minimal_env[3] = NULL;
	env_list = strings_to_list(minimal_env);
	free(cwd);
	return (env_list);
}

void	increment_shlvl(t_list *env_list)
{
	t_list *current;
	char *current_shlvl = NULL;
	char *new_shlvl_str = NULL;
	int shlvl_value;

	current = env_list;
	shlvl_value = 1;
	current_shlvl = NULL;
	new_shlvl_str = NULL;
	while (current)
	{
		if (ft_strncmp(current->content, "SHLVL=", 6) == 0)
		{
			current_shlvl = (char *)current->content + 6;
			shlvl_value = ft_atoi(current_shlvl) + 1;
			new_shlvl_str = ft_itoa(shlvl_value);
			current->content = ft_strjoin("SHLVL=", new_shlvl_str);
			return ;
		}
		current = current->next;
	}
	new_shlvl_str = ft_strdup("SHLVL=1");
	ft_lstadd_back(&env_list, ft_lstnew(new_shlvl_str));
}
