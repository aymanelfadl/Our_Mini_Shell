#include "minishell.h"

void push_back(t_list **export_envp, char *splited_export)
{
    char *key;
    char *value;
    char *export_envp_content;

    key = get_key(splited_export);
    if (key_is_already_exist(*export_envp, key))
    {
        export_envp_content = key_is_already_exist(*export_envp, key)->content;
        if (export_operator(splited_export) == 1)
        {
            export_envp_content = ft_substr(export_envp_content, 0, ft_strchr(export_envp_content, '=') + 1 - export_envp_content);
            export_envp_content = ft_strjoin(export_envp_content, ft_strchr(splited_export, '=') + 1);
        }
        else if (export_operator(splited_export) == 2)
        {

            export_envp_content = ft_substr(export_envp_content, 0, ft_strlen(export_envp_content));
            export_envp_content = ft_strjoin(export_envp_content, ft_strchr(splited_export, '=') + 1);
        }
        key_is_already_exist(*export_envp, key)->content = export_envp_content;
        return;
    }
    if (!(*(ft_strchr(splited_export, '=') + 1)))
        value = ft_strdup("=");
    else
        value = ft_strdup(ft_strchr(splited_export, '='));
    ft_lstadd_back(export_envp, ft_lstnew(ft_strjoin(key, value)));
}
