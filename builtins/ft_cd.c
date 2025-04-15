#include "minishell.h"

int ft_cd(t_tree *node)
{

    if (double_char_size(node->args) > 2)
        printf("bash: cd: too many arguments\n");
    if (!node->args[1])
        return (1);
    if(chdir(node->args[1]) == -1)
        perror(node->args[1]);
    return (0);
}