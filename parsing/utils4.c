#include "minishell.h"

static int quotes_strlen(char *s)
{
    int i;

    i = 0;
    while(s[i])
    {
        if ((s[i] != 34) || (s[i] != 39))
            i++;
        s++;
    }
    return (i);
}
char* handle_commandes_quoets(t_tree *tree)
{
    char *command;
    char *rest;
    char *command_with_no_quotes;
    int i;

    i = 0;
    command = skip_spaces(tree->data);
    if(my_strchr(command , " \t"))
    {
        rest = (my_strchr(command , " \t") );  
        command = ft_substr(command , 0 , my_strchr(command , " \t") - command);
    }
    if(!commas_ops_check(command))
        return (tree->data);
    command_with_no_quotes = ft_malloc(quotes_strlen(command) + 1);
    while (*command)
    {
        if (*command != 34 && *command != 39)
            command_with_no_quotes[i++] = *command;
        command++;
    }
    command[i] = 0;
    return(ft_strjoin(command_with_no_quotes , rest));
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