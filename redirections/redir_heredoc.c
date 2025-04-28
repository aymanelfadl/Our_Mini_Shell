#include "minishell.h"

static char *read_heredoc(char *delimiter)
{
	char *line;
	char *content;
	char *temp;

	content = ft_strdup("");
	if (!content)
		return (NULL);
	while (1)
	{
		line = readline("> ");
		if (!line || !ft_strcmp(line, delimiter))
		{
			free(line);
			break;
		}
		temp = ft_strjoin(content, line);
		free(content);
		free(line);
		if (!temp)
			return (NULL);
		content = ft_strjoin(temp, "\n");
		free(temp);
		if (!content)
			return (NULL);
	}
	return (content);
}

int redir_heredoc(t_redirection *r)
{
	char *content;
	int pipe_fd[2];
	t_redirection *current;

	current = r;
	while (current && current->type == REDIR_HEREDOC)
	{
		content = read_heredoc(current->file);
		if (!content)
			return (perror("Heredoc memory allocation error"), 1);
		if (current->next && current->next->type == REDIR_HEREDOC)
			current = current->next;
		else
			break;
	}
	if (pipe(pipe_fd) == -1)
		return (perror("Heredoc pipe creation error"), 1);
	
	write(pipe_fd[1], content, ft_strlen(content));
	close(pipe_fd[1]);  // Close write end of pipe
	
	// Redirect stdin to read from the pipe
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);  // Close original read fd as it's now duplicated

	// Skip any remaining heredocs in the list as we've processed them all
	while (r->next && r->next->type == REDIR_HEREDOC)
		r->next = r->next->next;
	
	return (0);
}