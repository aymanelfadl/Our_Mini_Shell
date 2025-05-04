#include "minishell.h"

static int ft_isnumber(char *str)
{
    if (*str == '-' || *str == '+')
        str++;
    while (*str)
    {
        if (!ft_isdigit(*str))
            return 0;
        str++;
    }
    return 1;
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
    int exit_code = 0;
    if (ft_count_args(node) > 2)
    {
        ft_putstr_fd("exit\n", STDOUT_FILENO);
        ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
        *get_exit_status() = 1; 
        return ;
    }
    if (ft_count_args(node) == 2 && !ft_isnumber(node->args[1]))
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(node->args[1], STDERR_FILENO);
        ft_putstr_fd(": numeric argument required", STDERR_FILENO);
        *get_exit_status() = 2;
        exit(2);
    }
    if (ft_count_args(node) == 2)
        exit_code = ft_atoi(node->args[1]);
    if (!node->parent)
        ft_putstr_fd("exit\n", STDOUT_FILENO);
    *get_exit_status() = exit_code;
    exit(exit_code);
}
