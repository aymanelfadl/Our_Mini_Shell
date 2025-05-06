#include "minishell.h"

int	ft_isnumber(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (0);
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static int	ft_count_args(t_tree *node)
{
	int	count;

	count = 0;
	while (node && node->args && node->args[count])
		count++;
	return (count);
}

static void	handle_numeric_exit(t_tree *node)
{
	long long	exit_code;
	
	if (ft_count_args(node) > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		*get_exit_status() = 1;
		return ;
	}
	exit_code = ft_atoi(node->args[1]);
	*get_exit_status() = exit_code & 255;
	close_saved_fds();
	exit(exit_code & 255);
}

void	ft_exit(t_tree *node)
{
	long long	exit_code;
	char		*arg;

	if (!node->parent)
		ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (ft_count_args(node) <= 1)
	{
		exit_code = *get_exit_status();
		close_saved_fds();
		exit(exit_code & 255);
	}
	arg = node->args[1];
	if (!ft_isnumber(arg))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		*get_exit_status() = 2;
		close_saved_fds();
		exit(2);
	}
	handle_numeric_exit(node);
}
