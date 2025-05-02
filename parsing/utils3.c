#include <minishell.h>

char *replace_strin_in_string(char *s, int start_string, int end_string, char *inserted_string)
{
    char *new_s;
    new_s = ft_substr(s, 0, start_string);
    new_s = ft_strjoin(new_s, inserted_string);
    new_s = ft_strjoin(new_s, s + end_string);
    return (new_s);
}

enum inside_what string_is_inside(char *s, int start)
{
    int i;
    int dbl_quotes;
    int one_quote;

    i = 0;
    dbl_quotes = 0;
    one_quote = 0;
    while (s[i] && i < start)
    {
        if (s[i] == 34)
            dbl_quotes++;
        if (s[i++] == 39)
            one_quote++;
    }
    if ((dbl_quotes % 2 == 0) && (one_quote % 2 == 0))
        return (INSIDE_NOTHING);
    else if ((dbl_quotes % 2 == 1) && (one_quote % 2 == 0))
        return (DOUBLE_QUOTES);
    else if ((dbl_quotes % 2 == 0) && (one_quote % 2 == 1))
        return (ONE_QUOTE);
    while (s[i] != 34 && s[i] != 39)
        i--;
    if (s[i] == 34)
        return (ONE_QUOTE);
    return (DOUBLE_QUOTES);
}

char *get_value(t_list *envp, char *key)
{
    t_list *value_node;
    char *content;

    if (key_is_already_exist(envp, key))
    {
        value_node = key_is_already_exist(envp, key);
        content = value_node->content;
        if (ft_strchr(content, '=') && *(ft_strchr(content, '=') + 1))
            return (ft_strdup(ft_strchr(content, '=') + 1));
        else
            return (NULL);
    }
    return (NULL);
}

char *handle_ted(t_list *envp, char *command)
{
    char *command_head = command;
    char *ted_adresse;

    ted_adresse = ft_strchr(command, '~');
    while (ted_adresse)
    {
        if (string_is_inside(command, ted_adresse - command) == INSIDE_NOTHING)
        {
            if ((ted_adresse == command_head) && (my_strchr(ted_adresse, " \t") == (ted_adresse + 1)))
            {
                command = replace_strin_in_string(command, ted_adresse - command, ted_adresse - command + 1, get_value(envp, "HOME"));
                ted_adresse = ft_strchr(command, '~');
            }
            else if ((*(ted_adresse - 1) == ' ' || *(ted_adresse - 1) == '\t') && (my_strchr(ted_adresse, " \t") == (ted_adresse + 1)))
            {
                command = replace_strin_in_string(command, ted_adresse - command, ted_adresse - command + 1, get_value(envp, "HOME"));
                ted_adresse = ft_strchr(command, '~');
            }
            else
                ted_adresse = ft_strchr(ted_adresse + 1, '~');
        }
        else
            ted_adresse = ft_strchr(ted_adresse + 1, '~');
    }
    return (command);
}
char *parse_env(char *s, t_list *envp)
{
    char *dollr_sign;
    int i = 1;

    s = handle_ted(envp, s);
    dollr_sign = ft_strchr(s, '$');
    while (dollr_sign)
    {
        i = 1;
        if (*(dollr_sign + 1) == '?')
        {
            s = replace_strin_in_string(s, (int)(dollr_sign - s), (dollr_sign - s + 2), ft_itoa(*get_exit_status()));
            dollr_sign = ft_strchr(s, '$');

        }
        else if (((ft_isalnum(*(dollr_sign + 1))) || (*(dollr_sign + 1) == '_')))
        {
            if (string_is_inside(s, (int)(dollr_sign - s)) == DOUBLE_QUOTES || string_is_inside(s, (int)(dollr_sign - s)) == INSIDE_NOTHING)
            {
                while (ft_isalnum(dollr_sign[i]))
                    i++;
                char *to_replace = get_value(envp, ft_substr(dollr_sign + 1, 0, i - 1));
                s = replace_strin_in_string(s, (int)(dollr_sign - s), (dollr_sign - s + i), to_replace);
                dollr_sign = ft_strchr(s, '$');
            }
            else
                dollr_sign = ft_strchr(dollr_sign + 1, '$');
        }
        else
            dollr_sign = ft_strchr(dollr_sign + 1, '$');
    }
    return (s);
}
