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

int read_heredoc_to_pipe(char *delimiter, int expand)
{
	int		pipe_fds[2];
	char	*line;
	char	*line_to_write;

	if (pipe(pipe_fds) == -1)
	{
		perror("minishell: pipe error");
		return (-1);
	}
	
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document delimited by end-of-file (wanted `", 2);
			ft_putstr_fd((char *)delimiter, 2);
			ft_putstr_fd("')\n", 2);
			break;
		}

		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break;
		}
		
		line_to_write = line;
		if (expand)
		{
			char *expanded = expand_heredoc_line(line);
			if (expanded != line)
			{
				line_to_write = expanded;
			}
			else
				free(expanded);
		}
		
		if (write(pipe_fds[1], line_to_write, ft_strlen(line_to_write)) == -1 ||
			write(pipe_fds[1], "\n", 1) == -1)
		{
			perror("minishell: write to heredoc pipe");
			if (expand && line_to_write != line)
				free(line_to_write);
			free(line);
			close(pipe_fds[0]);
			close(pipe_fds[1]);
			return (-1);
		}
		
		if (expand && line_to_write != line)
			free(line_to_write);
		free(line);
	}
	
	close(pipe_fds[1]);
	return (pipe_fds[0]);
}

int process_all_heredocs(t_tree *node)
{
	if (!node)
		return (0);

	if (process_all_heredocs(node->left) == -1) return (-1);
	if (process_all_heredocs(node->right) == -1) return (-1);

	if (node->type == COMMAND)
	{
		t_redirection *redir = node->redirects;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
			{
				if (redir->heredoc_fd > 0)
				{
					close(redir->heredoc_fd);
					redir->heredoc_fd = -1;
				}

				int heredoc_fd = read_heredoc_to_pipe(redir->file, redir->expand_heredoc);
				if (heredoc_fd == -1)
				{
					return (-1);
				}
				
				redir->heredoc_fd = heredoc_fd;
			}
			redir = redir->next;
		}
	}
	return (0);
}

void cleanup_heredoc_fds(t_tree *node)
{
	if (!node)
		return;

	cleanup_heredoc_fds(node->left);
	cleanup_heredoc_fds(node->right);

	if (node->type == COMMAND)
	{
		t_redirection *redir = node->redirects;
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
