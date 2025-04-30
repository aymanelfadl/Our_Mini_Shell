#include "minishell.h"

static char	*handle_exit_status(char **result_ptr, char *start)
{
	char	*exit_status;
	char	*temp;
	char	*after_var;
	char	*first_half;
	char	*final;

	exit_status = ft_itoa(*get_exit_status());
	temp = ft_substr(*result_ptr, 0, start - *result_ptr);
	after_var = ft_strdup(start + 2);
	first_half = ft_strjoin(temp, exit_status);
	final = ft_strjoin(first_half, after_var);
	free(exit_status);
	free(temp);
	free(after_var);
	free(first_half);
	free(*result_ptr);
	*result_ptr = final;
	return (*result_ptr);
}

static char	*handle_var_replacement(char **result_ptr, char *start, char *var_value, int var_len)
{
	char	*temp;
	char	*after_var;
	char	*first_half;
	char	*final;

	temp = ft_substr(*result_ptr, 0, start - *result_ptr);
	after_var = ft_strdup(start + var_len);
	first_half = ft_strjoin(temp, var_value ? var_value : "");
	final = ft_strjoin(first_half, after_var);
	free(temp);
	free(after_var);
	free(first_half);
	free(*result_ptr);
	*result_ptr = final;
	return (*result_ptr);
}

static char	*process_var(char **result_ptr, char *start)
{
	int		i;
	char	*var_name;
	char	*var_value;
	t_list	*env_list;

	if (*(start + 1) == '?')
		return (handle_exit_status(result_ptr, start));
	i = 1;
	while (start[i] && (ft_isalnum(start[i]) || start[i] == '_'))
		i++;
	if (i <= 1)
		return (start + 1);
	var_name = ft_substr(start + 1, 0, i - 1);
	env_list = initialize_env_list(NULL);
	var_value = get_value(env_list, var_name);
	handle_var_replacement(result_ptr, start, var_value, i);
	free(var_name);
	if (var_value)
		free(var_value);
	return (*result_ptr);
}

char	*expand_heredoc_line(char *line)
{
	char	*result;
	char	*start;

	if (!ft_strchr(line, '$'))
		return (ft_strdup(line));
	result = ft_strdup(line);
	if (!result)
		return (NULL);
	start = result;
	while ((start = ft_strchr(start, '$')))
	{
		if (*(start + 1) == '\0')
			break ;
		start = process_var(&result, start);
	}
	return (result);
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
		if (ft_strcmp(line, (char*)delimiter) == 0)
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
