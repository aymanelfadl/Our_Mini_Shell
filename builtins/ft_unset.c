#include "minishell.h"

void remove_from_env(const char *key)
{
    int i = 0;
    int j = 0;
    char **old_env = get_envp(NULL);
    char **new_env;

    while (old_env[i])
        i++;

    new_env = ft_malloc(sizeof(char *) * (i + 1));
    if (!new_env)
        return;
    i = 0;
    while (old_env[i])
    {
        if (ft_strncmp(old_env[i], key, ft_strlen(key)) == 0 && old_env[i][ft_strlen(key)] == '=')
        {
            i++;
            continue;
        }
        new_env[j++] = ft_strdup(old_env[i]);
        i++;
    }
    new_env[j] = NULL;
    get_envp(new_env);
}

int ft_unset(t_tree *node)
{
    int i;
    char *value;
 
    if (!node->args || !node->args[1])
        return 1;
    i = 1;
    while (node->args[i])
    {
        value = getenv(node->args[i]);
        if (value) 
            remove_from_env(node->args[i]);
        i++;
    }
    return 0;
}