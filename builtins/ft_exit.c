#include "minishell.h"

int	ft_isnumber(char *str)
{
	int sign;
	unsigned long long result;
	unsigned long long limit;

	result = 0;
	sign = 1;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (sign == -1)
		limit = (unsigned long long)(-(LLONG_MIN + 1)) + 1;
	else
		limit = (unsigned long long)LLONG_MAX;
	while (*str)
	{
		result = result * 10 + (*str - '0');
		if (result > limit)
			return (0);
		str++;
	}
	return (1);
}


static int ft_count_args(t_tree *node)
{
    int count = 0;

    while (node && node->args[count])
        count++;
    return count;
}

void ft_exit(t_tree *node)
{
    int exit_code;

    exit_code = 0;
    if (ft_count_args(node) > 1 && !ft_isnumber(node->args[1]))
    {
        *get_exit_status() = 2;
        ft_putstr_fd("exit\nminishell: ", STDERR_FILENO);
        ft_putstr_fd(node->args[1], STDERR_FILENO);
        ft_putstr_fd(": numeric argument required", STDERR_FILENO);
        exit(2);
    }
    if (ft_count_args(node) > 2)
    {
        *get_exit_status() = 1; 
        ft_putstr_fd("exit\nminishell: ", STDERR_FILENO);
        return ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
    }
    if (ft_count_args(node) == 2)
    {
        exit_code = ft_atoi(node->args[1]);
        printf("::%d\n", exit_code);
    }
    if (!node->parent)
        ft_putstr_fd("exit\n", STDOUT_FILENO);
    if (exit_code < 0)
        exit(156);
    *get_exit_status() = exit_code;
    exit(exit_code);
}
