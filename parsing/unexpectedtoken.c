#include <minishell.h>

static int	there_is_only_op_and__checklast_op(char *command)
{
	char	*head_command;

	head_command = command;
	while (find_next_ops(head_command) != -1)
	{
		head_command += find_next_ops(head_command);
		head_command = skip_ops(head_command);
	}
	if (*head_command == 0)
		return (1);
	command = skip_spaces(command);
	while (*command)
	{
		if (get_data_type(command) == NON)
			return (0);
		else
			command = skip_ops(command);
		command = skip_spaces(command);
	}
	return (1);
}

static int	check_two_ops(enum data_type op1, enum data_type op2)
{
	if (op1 == op2)
		return (0);
	if ((op1 == OR || op1 == AND || op1 == PIPE) && (op2 == OR || op2 == AND
			|| op2 == PIPE))
		return (0);
	if ((op1 == INPUT_REDIRECTION || op1 == OUTPUT_REDIRECTION
			|| op1 == APP_OUTPUT_REDIRECTION || op1 == APP_INPUT_REDIRECTION)
		&& (op2 == OR || op2 == AND || op2 == PIPE))
		return (0);
	return (1);
}

static int	check_condition(char *command, int first_loop, int n_condition)
{
	if (n_condition == 1)
		return (first_loop && ((get_data_type(command) == OR)
				|| (get_data_type(command) == AND)
				|| get_data_type(command) == PIPE));
	else if (n_condition == 2)
		return (get_data_type(command) == UNEXPECTED_TOKEN);
	return (get_data_type(command) != NON
		&& get_data_type(skip_ops(command)) != NON);
}

static int	apply_conditions(char *command, int first_loop)
{
	if (check_condition(command, first_loop, 1))
		return (0);
	else if (check_condition(command, first_loop, 2))
		return (0);
	else if (check_condition(command, first_loop, 3))
	{
		if (!check_two_ops(get_data_type(command),
				get_data_type(skip_ops(command))))
			return (0);
	}
	return (1);
}

int	check_unexpected_token(char *command)
{
	int	first_loop;

	first_loop = 1;
	if (!commas_ops_check(command)
		|| there_is_only_op_and__checklast_op(command))
		return (0);
	command = skip_spaces(command);
	while (*command)
	{
		if (apply_conditions(command, first_loop) == 0)
			return (0);
		command = skip_ops(command);
		first_loop = 0;
		if (find_next_ops(command) != -1)
			command += find_next_ops(command);
		else
			break ;
	}
	return (1);
}
