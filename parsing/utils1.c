#include <minishell.h>

int ft_free(t_list *lst)
{
    t_list *helper;
    while (lst)
    {
        free(lst->content);
        helper = lst;
        lst = lst->next;
        free(helper);
    }
    return (1);
}

void *ft_malloc(size_t size)
{
    t_list *new;
    new = malloc(sizeof(t_list));
    new->content = malloc(size);
    ft_bzero(new->content, size);
    new->next = NULL;
    ft_lstadd_back(&garbage_collector, new);
    return (new->content);
}

void add_adresse_to_garbage(void *adress)
{
    t_list *new;

    new = malloc(sizeof(t_list));
    new->content = adress;
    new->next = NULL;
    ft_lstadd_back(&garbage_collector, new);
}

int double_char_size(char **s)
{
    int i;
    i = 0;
    while (s && *s)
    {
        s++;
        i++;
    }
    return (i);
}
