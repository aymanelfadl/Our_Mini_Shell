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

static int	ft_strtoll_check_sign(const char *str, int *i, int *sign)
{
	*sign = 1;
	if (str[*i] == '+' || str[*i] == '-')
	{
		if (str[*i] == '-')
			*sign = -1;
		(*i)++;
	}
	if (!str[*i])
		return (0);
	return (1);
}

int	ft_strtoll(const char *str, long long *result)
{
	int					sign;
	unsigned long long	num;
	int					i;
	int					digit;

	i = 0;
	num = 0;
	*result = 0;
	if (!ft_strtoll_check_sign(str, &i, &sign))
		return (0);
	while (str[i] >= '0' && str[i] <= '9')
	{
		digit = str[i] - '0';
		if (sign == 1 && num > (LLONG_MAX - digit) / 10)
			return (0);
		if (sign == -1 && num > ((unsigned long long)LLONG_MAX + 1 - digit) / 10)
			return (0);
		num = num * 10 + digit;
		i++;
	}
	if (str[i] != '\0')
		return (0);
	if (sign)
	*result = (long long)num;
	else
	*result = -(long long)num;
	return (1);
}

static void	ft_exit_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
	*get_exit_status() = 2;
	close_saved_fds();
	exit(2);
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
	if (!ft_strtoll(arg, &exit_code))
		ft_exit_error(arg);
	if (ft_count_args(node) > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		*get_exit_status() = 1;
		return ;
	}
	*get_exit_status() = exit_code & 255;
	close_saved_fds();
	exit(exit_code & 255);
}

