#include "minishell.h"

int builtins_engine(t_tree *node)
{
    if (!ft_strcmp(node->args[0], (const char *)"exit"))
        return (ft_exit(node), 0);    
    else if (!ft_strcmp(node->args[0], (const char *)"env"))
        return (ft_env(node));
    else if (!ft_strcmp(node->args[0], (const char *)"unset"))
        return (ft_unset(node));
    else if (!ft_strcmp(node->args[0], (const char *)"cd"))
        return ft_cd(node);
    else if (!ft_strcmp(node->args[0], (const char *)"echo"))
        return ft_echo(node);
    else if (!ft_strcmp(node->args[0], (const char *)"pwd"))
        return ft_pwd();
    // else if (!ft_strcmp(node->args[0], (const char *)"export")) i have no  idea what is the second arg here 
        // return ft_export(node, NULL);
    return -1;   
}