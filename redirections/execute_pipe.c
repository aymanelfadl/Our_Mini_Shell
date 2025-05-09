/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:29:06 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:29:06 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	wait_for_child(pid_t child_pid)
{
	int	status;

	if (child_pid <= 0)
		return (1);
	if (waitpid(child_pid, &status, 0) == -1)
	{
		perror("waitpid");
		return (1);
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

void	execute_left(t_tree *node)
{
	dup2(node->pipe_fds[1], STDOUT_FILENO);
	close(node->pipe_fds[0]);
	close(node->pipe_fds[1]);
	close_saved_fds();
	exit(execute_node(node->left));
}

void	execute_right(t_tree *node)
{
	dup2(node->pipe_fds[0], STDIN_FILENO);
	close(node->pipe_fds[1]);
	close(node->pipe_fds[0]);
	if (!node->right)
	{
		apply_redirections(node->redirects);
		close_saved_fds();
		exit(EXIT_SUCCESS);
	}
	close_saved_fds();
	exit(execute_node(node->right));
}

int	execute_pipe(t_tree *node)
{
	pid_t	l;
	pid_t	r;

	if (pipe(node->pipe_fds) < 0)
		return (perror("pipe"), 1);
	l = fork();
	if (l == 0)
		execute_left(node);
	r = fork();
	if (r == 0)
		execute_right(node);
	close(node->pipe_fds[0]);
	close(node->pipe_fds[1]);
	waitpid(l, NULL, 0);
	return (wait_for_child(r));
}
