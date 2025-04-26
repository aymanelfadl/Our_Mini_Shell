#include <minishell.h>

char *replace_strin_in_string(char *s, int start_string, int end_string, char *inserted_string)
{
    char *new_s;
    new_s = ft_substr(s, 0, start_string);
    printf("new_s = %s \n",new_s);
    new_s = ft_strjoin(new_s, inserted_string);
    printf("new_s = %s \n",new_s);
    new_s = ft_strjoin(new_s, s + end_string);
    printf("new_s = %s \n",new_s);
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