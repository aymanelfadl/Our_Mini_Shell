#include "minishell.h"




int ft_env(t_tree *node)
{
    char **env = get_envp(NULL);
    int i = 0;
    while (env[i])
    {
        printf("%s\n", env[i]);
        i++;
    }
    return 0;
}