#include "minishell.h"

static int handle_builtin(t_tree *node, t_list *envp)
{
    if (!ft_strcmp(node->args[0], "exit"))
        ft_exit(node);
    else if (!ft_strcmp(node->args[0], "unset"))
        return ft_unset(node);
    else if (!ft_strcmp(node->args[0], "cd"))
        return ft_cd(node, envp);
    else if (!ft_strcmp(node->args[0], "echo"))
        return (ft_echo(node), 0);
    else if (!ft_strcmp(node->args[0], "pwd"))
        return (ft_pwd(), 0);
    else if (!ft_strcmp(node->args[0], "export"))
        return (ft_export(node, &envp) , 0);
    else if (!ft_strcmp(node->args[0], "env"))
        return ft_env(node); 

    return -1;
}

int builtins_engine(t_tree *node, t_list *envp)
{
    if (!node || !node->args || !node->args[0])
        return -1;

    return handle_builtin(node, envp);
}