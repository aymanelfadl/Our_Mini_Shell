#include "minishell.h"

static char *select_arg(char *old_envp, t_tree *node)
{
    int  i;
    int count; 

    count = 0;
    i = 0;
    while (node->args[count])
        count++;
    while (i < count)
    {
        if (!ft_strcmp(old_envp, node->args[i]))
            return NULL;
        i++;
    }
    return old_envp;
}


static char **set_new_envp(char **old_envp, t_tree *node)
{
    int count;
    int new_index;
    int i;
    char **new_envp;
    char *selected_arg;

    i = -1;
    new_index = 0;
    count = 0;

    while (old_envp[count])
        count++;
    new_envp = ft_malloc((count + 1) * sizeof(char *));
    if (!new_envp)
        return NULL;
    while (++i < count)
    {
        selected_arg = select_arg(old_envp[i], node);
        if (!selected_arg)
            new_envp[new_index++] = old_envp[i];
    }
    new_envp[new_index] = NULL;
    return new_envp;
}

int ft_unset(t_tree *node)
{
    char **res = set_new_envp(get_envp(NULL), node);
    get_envp(res);
    return 0;
}