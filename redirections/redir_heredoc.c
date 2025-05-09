/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:29:06 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:29:06 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_heredoc_line(int pipe_fd, char *line, int expand)
{
	char	*line_to_write;
	char	*expanded;

	line_to_write = line;
	if (expand)
	{
		expanded = expand_heredoc_line(line);
		if (expanded != line)
			line_to_write = expanded;
	}
	if (write_line_to_pipe(pipe_fd, line_to_write) == -1)
		return (-1);
	if (line_to_write != line)
		free(line_to_write);
	return (0);
}

static int	read_heredoc_lines(int *pipe_fds, const char *delimiter, int expand)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, (char *)delimiter) == 0)
		{
			if (!line)
				handle_eof_warning(delimiter);
			free(line);
			break ;
		}
		if (process_heredoc_line(pipe_fds[1], line, expand) == -1)
		{
			free(line);
			close(pipe_fds[1]);
			return (-1);
		}
		free(line);
	}
	return (0);
}

static void	handle_parent_process(int *pipe_fds, int *read_fd, pid_t child_pid)
{
	int	status;

	close(pipe_fds[1]);
	*read_fd = pipe_fds[0];
	waitpid(child_pid, &status, 0);
	signal(SIGINT, sigint_handler);
	if (WIFSIGNALED(status))
	{
		close(*read_fd);
		*get_exit_status() = 130;
		*read_fd = -1;
	}
}

static void	setup_heredoc_child(int *pipe_fds, const char *delimiter,
		int expand)
{
	signal(SIGINT, SIG_DFL);
	close(pipe_fds[0]);
	close_saved_fds();
	if (read_heredoc_lines(pipe_fds, delimiter, expand) == -1)
	{
		close(pipe_fds[1]);
		exit(1);
	}
	close(pipe_fds[1]);
	exit(0);
}

int	read_heredoc_to_pipe(const char *delimiter, int expand)
{
	int		pipe_fds[2];
	int		read_fd;
	pid_t	child_pid;

	signal(SIGINT, heredoc_sigint_handler);
	if (pipe(pipe_fds) == -1)
		return (perror("minishell: pipe error"), -1);
	child_pid = fork();
	if (child_pid < 0)
		return (signal(SIGINT, sigint_handler), perror("minishell: fork error"),
			close(pipe_fds[0]), close(pipe_fds[1]), -1);
	if (child_pid == 0)
		setup_heredoc_child(pipe_fds, delimiter, expand);
	else
		handle_parent_process(pipe_fds, &read_fd, child_pid);
	return (read_fd);
}
