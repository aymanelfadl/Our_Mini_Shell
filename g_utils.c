/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   g_utils.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:28:49 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:28:49 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	*get_std_fds(int stdin_fd, int stdout_fd)
{
	static int	fds[2] = {-1, -1};

	if (stdin_fd >= 0 && stdout_fd >= 0)
	{
		fds[0] = stdin_fd;
		fds[1] = stdout_fd;
	}
	return (fds);
}

void	save_std_fds(int *saved_stdin, int *saved_stdout)
{
	*saved_stdin = dup(STDIN_FILENO);
	*saved_stdout = dup(STDOUT_FILENO);
	get_std_fds(*saved_stdin, *saved_stdout);
}

void	close_saved_fds(void)
{
	int	*fds;

	fds = get_std_fds(-1, -1);
	if (fds[0] >= 0)
		close(fds[0]);
	if (fds[1] >= 0)
		close(fds[1]);
}

void	restore_std_fds(int saved_stdin, int saved_stdout)
{
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
}
