#include "minishell.h"

static int	read_heredoc_lines(int *pipe_fds, const char *delimiter, int expand)
{
	char	*line;
	char	*line_to_write;

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			handle_eof_warning(delimiter);
			free(line);
			break;
		}
		if (ft_strcmp(line, (char*) delimiter) == 0)
		{
			free(line);
			break;
		}
		line_to_write = line;
		if (expand)
		{
			char *expanded = expand_heredoc_line(line);
			if (expanded != line)
				line_to_write = expanded;
		}
		if (write_line_to_pipe(pipe_fds[1], line_to_write) == -1)
			return (close(pipe_fds[1]), -1);
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

static void	setup_heredoc_child(int *pipe_fds, const char *delimiter, int expand)
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
