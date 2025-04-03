#include "minishell.h"

int builtins_engine(t_tree *node)
{
    if (!ft_strcmp(node->args[0], (const char *)"unset"))
        return ft_unset(node);
    return -1;   
}