#include "minishell.h"

int ft_env(t_tree *node)
{
    char **env = get_envp(NULL);

    while (env && *env)
    {
        printf("%s\n", *env);
        env++;
    }
    return 0;
}