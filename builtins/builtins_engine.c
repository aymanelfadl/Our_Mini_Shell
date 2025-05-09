/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_engine.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:02:28 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:02:28 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_builtin(t_tree *node, t_list *envp)
{
	if (!ft_strcmp(node->args[0], "exit"))
		return (ft_exit(node), *get_exit_status());
	else if (!ft_strcmp(node->args[0], "unset"))
		return (ft_unset(node));
	else if (!ft_strcmp(node->args[0], "cd"))
		return (ft_cd(node, envp));
	else if (!ft_strcmp(node->args[0], "echo"))
		return (ft_echo(node), 0);
	else if (!ft_strcmp(node->args[0], "pwd"))
		return (ft_pwd(envp));
	else if (!ft_strcmp(node->args[0], "export"))
		return (ft_export(node, &envp));
	else if (!ft_strcmp(node->args[0], "env"))
		return (ft_env(node));
	return (-1);
}

int	builtins_engine(t_tree *node, t_list *envp)
{
	if (!node || !node->args || !node->args[0])
		return (-1);
	return (handle_builtin(node, envp));
}
