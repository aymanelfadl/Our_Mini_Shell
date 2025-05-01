#include "minishell.h"

static int ft_isnumber(char *str)
{
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
        printf("exit: too many arguments\n");
        *get_exit_status() = 1; 
        return;
    }
    if (ft_count_args(node) == 2 && !ft_isnumber(node->args[1]))
    {
        printf("exit: %s: numeric argument required\n", node->args[1]);
        exit(255);
    }
    if (ft_count_args(node) == 2)
        exit_code = ft_atoi(node->args[1]);

    if (!node->parent)
        printf("exit\n");

    ft_free(garbage_collector);
    exit(exit_code);
}