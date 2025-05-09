/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:02:28 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:02:28 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_count_args(t_tree *node)
{
	int	count;

	count = 0;
	while (node && node->args && node->args[count])
		count++;
	return (count);
}

static void	ft_exit_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
	*get_exit_status() = 2;
	close_saved_fds();
	exit(2);
}

static void	handle_exit_code(long long exit_code)
{
	close_saved_fds();
	if (exit_code < 0)
	{
		*get_exit_status() = 156;
		exit(156);
	}
	else
	{
		*get_exit_status() = exit_code;
		exit(exit_code);
	}
}

void	ft_exit(t_tree *node)
{
	long long	exit_code;
	char		*arg;

	if (!node->parent)
		ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (ft_count_args(node) <= 1)
	{
		exit_code = *get_exit_status();
		close_saved_fds();
		exit(exit_code);
	}
	arg = node->args[1];
	if (!ft_strtoll(arg, &exit_code))
		ft_exit_error(arg);
	if (ft_count_args(node) > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		*get_exit_status() = 1;
		return ;
	}
	handle_exit_code(exit_code);
}
