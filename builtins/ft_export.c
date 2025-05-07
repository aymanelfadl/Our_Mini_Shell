#include "minishell.h"

static t_list *ft_lstdup(t_list *src)
{
    t_list *dst;
    t_list *head;

    dst = ft_malloc(sizeof(t_list));
    head = dst;
    while (src)
    {
        dst->content = ft_strdup(src->content);
        if (src->next)
        {
            dst->next = ft_malloc(sizeof(t_list));
            dst = dst->next;
        }
        else
            dst->next = NULL;
        src = src->next;
    }
    return (head);
}
static void sort_envp(t_list *export_envp)
{
    t_list *temp;
    t_list *remmember_me;
    char *helper;

    helper = NULL;
    temp = NULL;
    while (export_envp)
    {
        temp = (export_envp)->next;
        remmember_me = (export_envp)->next;
        while (temp)
        {
            if (ft_strcmp((char *)(export_envp)->content, (char *)temp->content) > 0)
            {
                helper = ((export_envp)->content);
                (export_envp)->content = temp->content;
                temp->content = helper;
            }
            temp = temp->next;
        }
        export_envp = remmember_me;
    }
}

static void write_expoert_envp(t_list export_envp)
{
    char *content;
    int equal_found;
    t_list *make_copy;

    make_copy = ft_lstdup(&export_envp);
    sort_envp(make_copy);
    while (1)
    {
        equal_found = 0;
        content = make_copy->content;
        ft_putstr_fd("declare -x ", 1);
        content = write_envp_content(content , &equal_found);
        if (equal_found)
            ft_putchar_fd('\x22', 1);

        ft_putchar_fd('\n', 1);
        if (make_copy->next)
            make_copy = make_copy->next;
        else
            break;
    }
}

char *get_key(char *splited_export)
{
    char *equale_adresse;
    char *add_aquale_adresse;
    char *adresse;
    equale_adresse = ft_strchr(splited_export, '=');
    add_aquale_adresse = ft_strnstr(splited_export, "+=", ft_strlen(splited_export));
    if (add_aquale_adresse && (add_aquale_adresse < equale_adresse))
        adresse = add_aquale_adresse;
    else
        adresse = equale_adresse;
    return (ft_substr(splited_export, 0, adresse - splited_export));
}

int ft_export(t_tree *node, t_list **export_envp)
{
    char **splited_export;
    int exit_status;

    exit_status = 0;
    splited_export = node->args;
    if (double_char_size(node->args) == 1)
        write_expoert_envp(**export_envp);
    else
    {
        while (++splited_export && *splited_export)
        {
            if (**splited_export == 0)
                ft_putstr_fd("bash: export: `': not a valid identifier\n", 2);
            else
            {
                *splited_export = remove_quotes(*splited_export);
                export_switch_cases(*splited_export, export_envp, &exit_status);
            }
        }
    }
    return (exit_status);
}
