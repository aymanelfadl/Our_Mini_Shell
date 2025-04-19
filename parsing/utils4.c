#include <minishell.h>


char *handle_commandes_quoets(t_tree *tree)
{
    char *command;
    char *rest;
    char *command_with_no_quotes;
    int i;

    i = 0;
    command = skip_spaces(tree->data);
    if (my_strchr(command, " \t"))
    {
        rest = (my_strchr(command, " \t"));
        command = ft_substr(command, 0, my_strchr(command, " \t") - command);
    }
    if (!commas_ops_check(command))
        return (tree->data);
    command_with_no_quotes = remove_quotes(command);
    return (ft_strjoin(command_with_no_quotes, rest));
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