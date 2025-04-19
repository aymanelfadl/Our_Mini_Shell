#include "minishell.h"

int  valid_args(t_tree *node)
{
    int i;

    i = 1;
    while (node->args && node->args[i])
    {
        if (ft_strchr(node->args[i], '=') == NULL)
            return 0;
        i++;
    }
    return 1;

} 

int ft_env(t_tree *node)
{
    char **env = get_envp(NULL);
    int i;

    i = 0;
    if (valid_args(node))
    {
        while (env && *env)
        {
            printf("%s\n", *env);
            env++;
        }
        while (node->args && node->args[++i])
            printf("%s\n", node->args[i]);
        return 0;
    }
    else
        return -1;
}
