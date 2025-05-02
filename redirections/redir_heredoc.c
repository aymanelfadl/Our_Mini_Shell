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
			break;
		}
		if (ft_strcmp(line, (char*)delimiter) == 0)
			break;
		line_to_write = line;
		if (expand)
		{
			char *expanded = expand_heredoc_line(line);
			if (expanded != line)
				line_to_write = expanded;
		}
		if (write_line_to_pipe(pipe_fds[1], line_to_write) == -1)
			return (close(pipe_fds[0]), close(pipe_fds[1]), -1);
	}
	return (0);
}

int read_heredoc_to_pipe(const char *delimiter, int expand)
{
	int		pipe_fds[2];
	int		read_fd;

	if (pipe(pipe_fds) == -1)
	{
		perror("minishell: pipe error");
		return (-1);
	}
	if (read_heredoc_lines(pipe_fds, delimiter, expand) == -1)
		return (-1);
	close(pipe_fds[1]);
	read_fd = pipe_fds[0];
	return (read_fd);
}
