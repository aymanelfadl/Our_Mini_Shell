/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:02:28 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:02:28 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

char	*skip_spaces(char *s)
{
	while (*s && (*s == ' ' || *s == '\t'))
		s++;
	return (s);
}

static int	is_n_option(char *n_option)
{
	if (!(*n_option == '-'))
		return (0);
	n_option++;
	while (*n_option)
	{
		if (*n_option != 'n')
			return (0);
		n_option++;
	}
	return (1);
}

static void	ft_echo_write(char **to_write, int n_option)
{
	int	not_first_loop;

	not_first_loop = 0;
	while (to_write && *to_write)
	{
		if (not_first_loop)
			ft_putchar_fd(' ', 1);
		ft_putstr_fd(*to_write, 1);
		to_write++;
		not_first_loop = 1;
	}
	if (!n_option)
		ft_putchar_fd('\n', 1);
}

void	ft_echo(t_tree *node)
{
	int		n_option;
	int		i;

	i = 2;
	if (node->args && node->args[1] && is_n_option(node->args[1]))
		n_option = 1;
	else
		n_option = 0;
	if (n_option)
	{
		while (node->args[i] && is_n_option(node->args[i]))
			i++;
	}
	if (n_option)
		ft_echo_write((node->args) + i, n_option);
	else
		ft_echo_write((node->args) + 1, n_option);
	*get_exit_status() = 0;
}
