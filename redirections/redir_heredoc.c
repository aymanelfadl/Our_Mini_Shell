#include "minishell.h"
#include <signal.h>

char *expand_heredoc_line(char *line)
{
	if (ft_strchr(line, '$'))
	{
		if (ft_strncmp(line, "$USER", 5) == 0)
		{
			char *user = getenv("USER");
			char *rest = line + 5;
			if (user)
				return (ft_strjoin(user, rest));
		}
	}
	return ft_strdup(line);
}

static void	handle_eof_warning(const char *delimiter)
{
	ft_putstr_fd("minishell: warning: here-document delimited by ", 2);
	ft_putstr_fd("end-of-file (wanted `", 2);
	ft_putstr_fd((char *)delimiter, 2);
	ft_putstr_fd("')\n", 2);
}

static int	write_line_to_pipe(int pipe_fd, char *line_to_write, int expand, char *line)
{
	if (write(pipe_fd, line_to_write, ft_strlen(line_to_write)) == -1 ||
		write(pipe_fd, "\n", 1) == -1)
	{
		perror("minishell: write to heredoc pipe");
		if (expand && line_to_write != line)
			free(line_to_write);
		free(line);
		return (-1);
	}
	return (0);
}

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
			else
				free(expanded);
		}
		if (write_line_to_pipe(pipe_fds[1], line_to_write, expand, line) == -1)
			return (close(pipe_fds[0]), close(pipe_fds[1]), -1);
		if (expand && line_to_write != line)
			free(line_to_write);
		free(line);
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

static void	process_heredoc_redirects(t_tree *node)
{
	t_redirection	*redir;
	int				heredoc_fd;

	redir = node->redirects;
	while (redir)
	{
		if (redir->type == REDIR_HEREDOC)
		{
			if (redir->heredoc_fd > 0)
			{
				close(redir->heredoc_fd);
				redir->heredoc_fd = -1;
			}
			heredoc_fd = read_heredoc_to_pipe(redir->file, redir->expand_heredoc);
			if (heredoc_fd == -1)
				return;
			redir->heredoc_fd = heredoc_fd;
		}
		redir = redir->next;
	}
}

int	process_all_heredocs(t_tree *node)
{
	if (!node)
		return (0);

	if (process_all_heredocs(node->left) == -1)
		return (-1);
	if (process_all_heredocs(node->right) == -1)
		return (-1);

	if (node->type == COMMAND)
	{
		process_heredoc_redirects(node);
		/* Check if any heredoc failed by checking if any required heredoc fd is -1 */
		t_redirection *redir = node->redirects;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->heredoc_fd == -1)
				return (-1);
			redir = redir->next;
		}
	}
	return (0);
}

static void	cleanup_node_heredocs(t_tree *node)
{
	t_redirection	*redir;

	if (node->type == COMMAND)
	{
		redir = node->redirects;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->heredoc_fd > 0)
			{
				close(redir->heredoc_fd);
				redir->heredoc_fd = -1;
			}
			redir = redir->next;
		}
	}
}

void	cleanup_heredoc_fds(t_tree *node)
{
	if (!node)
		return;

	cleanup_heredoc_fds(node->left);
	cleanup_heredoc_fds(node->right);
	cleanup_node_heredocs(node);
}
