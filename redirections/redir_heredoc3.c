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
			break;
		start = process_var(&result, start);
	}
	return (result);
}

int	write_line_to_pipe(int pipe_fd, char *line_to_write, int expand, char *line)
{
	if (write(pipe_fd, line_to_write, ft_strlen(line_to_write)) == -1 ||
		write(pipe_fd, "\n", 1) == -1)
	{
		perror("minishell: write to heredoc pipe");
		return (-1);
	}
	return (0);
}