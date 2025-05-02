#include <minishell.h>

char *get_command(char *all_command)
{
    char *command_name;
    char *command_head;

    command_head = all_command;

    while (*all_command)
    {
        if (*all_command == ' ' || *all_command == '\t')
        {
            if (string_is_inside(all_command, ft_strlen(all_command)) == INSIDE_NOTHING)
            {
                command_name = ft_substr(command_head , 0 , all_command - command_head);
                return (command_name);
            }
        }
        all_command++;
    }
    return (command_head);
}
char *handle_commandes_quoets(t_tree *tree)
{
    char *command;
    char *rest;

    command = skip_spaces(tree->data);
    command = get_command(command);
    rest  = skip_spaces(tree->data) +ft_strlen(command);
    if (*skip_spaces(rest) == 0)
        rest = NULL;
    command = remove_quotes(command);
    command = ft_strjoin("'" , command);
    command = ft_strjoin(command,"'");
    return (ft_strjoin(command ,rest));
}

t_list *strings_to_list(char **strings)
{
    t_list *head;
    head = NULL;
    while (strings && *strings)
    {
        ft_lstadd_back(&head, ft_lstnew(*strings));
        strings++;
    }
    return (head);
}