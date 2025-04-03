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


char **set_new_envp(char **old_envp, t_tree *node)
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
    printf("hahah");
    while (++i < count)
    {
        selected_arg = select_arg(old_envp[i], node);
        if (!selected_arg)
            new_envp[new_index++] = old_envp[i];
    }
    new_envp[new_index] = NULL;
    return new_envp;
}

void ft_unset(t_tree *node)
{
    char **res = set_new_envp(get_envp(NULL), node);
    get_envp(res);
}

void builtins_engine(t_tree *node)
{
    if (!ft_strcmp(node->args[0], (const char *)"unset")) 
        ft_unset(node);
}

// Node path: (null)
// Node data: unset TERM
// Node args:
//   unset
//   TERM
// Node heredoc_content: (null)
// Left child: NULL
// Right child: NULL
// Parent: NULL
// Node type: 7
// The Exit Status :: -2

int main(int ac, char **av, char **envp)
{
    t_tree node;
    char *args[] = {"unset", "TERM", NULL};
    node.args = args;
    
    char **new_envp = set_new_envp(envp, &node);
    
    char **temp = new_envp;
    while (temp != NULL && *temp != NULL)
    {
        printf("%s\n", *temp);
        temp++;
    } 
    return 0;
}