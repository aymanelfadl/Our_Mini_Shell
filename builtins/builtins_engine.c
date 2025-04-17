#include "minishell.h"

// should some commands fork and oters no


int builtins_engine(t_tree *node)
{
    t_list *envp;

    envp = strings_to_list(get_envp(NULL));
    if (!ft_strcmp(node->args[0], (char *)"exit"))
        return (ft_exit(node), 0);    
    else if (!ft_strcmp(node->args[0], (char *)"env"))
        return (ft_env(node));
    else if (!ft_strcmp(node->args[0], (char *)"unset"))
        return (ft_unset(node));
    else if (!ft_strcmp(node->args[0], (char *)"cd"))
        return ft_cd(node, envp);
    else if (!ft_strcmp(node->args[0], (char *)"echo"))
        return (ft_echo(node));
    else if (!ft_strcmp(node->args[0], (char *)"pwd"))
    {
        if (ft_pwd() != NULL)
            return 0;
        else 
            return 1;
    }
    else if (!ft_strcmp(node->args[0], (char *)"export"))
        return (ft_export(node, &envp)); 
    return -1;   
}