#include "minishell.h"

char **list_to_char_array(t_list *list)
{
    int size = ft_lstsize(list);
    char **array = ft_malloc(sizeof(char *) * (size + 1));

    if (!array)
        return NULL;

    int i = 0;
    while (list)
    {
        array[i] = ft_strdup((char *)list->content);
        list = list->next;
        i++;
    }
    array[i] = NULL;

    return array;
}

size_t get_key_len(char *str)
{
    size_t i;
    
    i = 0;
    while (str[i])
    {
        if (str[i] == '=')
            return i;
        i++;
    }
    return i;
}

int is_unset_target(char *str, char **args)
{
    int i = 1;
    size_t len;

    while (args[i])
    {
        len = get_key_len(str);
        if (ft_strncmp(str, args[i], len) == 0 && str[len] == '=' && !ft_strchr(args[i], '='))
            return 1; 
        i++;
    }
    return 0;
}

int ft_unset(t_tree *node)
{
    t_list *old_env = initialize_env_list(NULL);
    t_list *new_env = NULL;
    char **envp;
    while (old_env)
    {
        if (!is_unset_target((char *)old_env->content, node->args))
            ft_lstadd_back(&new_env, ft_lstnew(ft_strdup((char *)old_env->content)));
        old_env = old_env->next;
    }
    envp = list_to_char_array(new_env);
    initialize_env_list(envp);
    return 0;    
}
