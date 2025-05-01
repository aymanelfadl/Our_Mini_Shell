#include "minishell.h"

// concatinate [ ]
// oan exesting key accept new value
//

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

    sort_envp(&export_envp);
    while (1)
    {
        equal_found = 0;
        content = export_envp.content;
        ft_putstr_fd("declare -x ", 1);
        while (*content)
        {
            ft_putchar_fd(*content, 1);
            if (*content == '=')
            {
                ft_putchar_fd('\x22', 1);
                equal_found = 1;
            }
            content++;
        }
        if (equal_found)
            ft_putchar_fd('\x22', 1);

        ft_putchar_fd('\n', 1);
        if (export_envp.next)
            export_envp = *(export_envp.next);
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

void ft_export(t_tree *node, t_list **export_envp)
{
    char **splited_export;
    char *key;
    char *value;

    splited_export = node->args;
    if (double_char_size(node->args) == 1)
        write_expoert_envp(**export_envp);
    else
    {
        splited_export++;
        *splited_export = remove_quotes(*splited_export);
        while (splited_export && *splited_export)
        {
            if (!ft_strchr(*splited_export, '=') && is_valid_key(*splited_export) && !key_is_already_exist(*export_envp, *splited_export))
            {
                ft_lstadd_back(export_envp, ft_lstnew(*splited_export));
            }
            else if ((ft_strnstr(*splited_export, "=", ft_strlen(*splited_export)) || ft_strnstr(*splited_export, "+=", ft_strlen(*splited_export))) && is_valid_key(*splited_export)) // 1 -> "="
                push_back(export_envp, *splited_export);
            else if (!is_valid_key(*splited_export))
                printf("export : %s : not a valid identifier\n", *splited_export);
            splited_export++;
        }
    }
}
