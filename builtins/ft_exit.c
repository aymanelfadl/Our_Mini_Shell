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
    int count;

    count = 0;
    while (node && node->args[count])
        count++;
    return count;
}
void ft_exit(t_tree *node)
{
    if (ft_count_args(node) > 2)
        printf("exit: too many arguments\n");
    else if (ft_count_args(node) == 1)
    {
        printf("exit\n");
        exit(EXIT_SUCCESS);
    }  
    else if (!ft_isnumber(node->args[1]))
        printf("exit: %s: numeric argument required\n", node->args[1]);
    else
    {
        printf("exit\n");
        exit(ft_atoi(node->args[1]));
    }
}