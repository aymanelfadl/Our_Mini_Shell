#include "minishell.h"

static int  valid_args(t_tree *node)
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
    t_list *env = initialize_env_list(NULL);
    int i;

    i = 0;
    if (valid_args(node))
    {
        while (env)
        {
            printf("%s\n", (char *)env->content);
            env = env->next;
        }
        while (node->args && node->args[++i])
            printf("%s\n", node->args[i]);
        return 0;
    }
    else
        return -1;
}
