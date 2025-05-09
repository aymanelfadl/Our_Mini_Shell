/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:02:28 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:02:28 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	valid_args(t_tree *node)
{
	int	i;

	i = 1;
	while (node->args && node->args[i])
	{
		if (ft_strchr(node->args[i], '=') == NULL)
			return (0);
		i++;
	}
	return (1);
}

int	ft_env(t_tree *node)
{
	t_list	*env;
	int		i;

	env = initialize_env_list(NULL);
	i = 0;
	if (valid_args(node))
	{
		while (env)
		{
			if (ft_strchr((char *)env->content, '='))
				printf("%s\n", (char *)env->content);
			env = env->next;
		}
		while (node->args && node->args[++i])
			printf("%s\n", node->args[i]);
		return (0);
	}
	else
		return (-1);
}
