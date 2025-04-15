#include "minishell.h"

int ops_size(char *s, char **all_ops)
{
    int i;

    i = 0;
    while (*s)
    {
        if (extract_ops_helper(s, all_ops) >= 0)
        {
            s += ft_strlen(all_ops[extract_ops_helper(s, all_ops)]);
            i++;
        }
        else
            s++;
    }
    return (i + 1);
}

int ft_strcmp(char *s1, char *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

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




char *parse_env(char *s)
{
    char *dollr_sign;
    int i = 1;

    dollr_sign = ft_strchr(s, '$');
    while (dollr_sign && ((ft_isalpha(*(dollr_sign + 1))) || (*(dollr_sign + 1 ) == '_')))
    {
        if (string_is_inside(s, (int)(dollr_sign - s)) == DOUBLE_QUOTES || string_is_inside(s, (int)(dollr_sign - s)) == INSIDE_NOTHING)
        {
            while (ft_isalpha(dollr_sign[i]))
                i++;
            char *to_replace = getenv(ft_substr(dollr_sign + 1, 0, i - 1));
            s = replace_strin_in_string(s, (int)(dollr_sign - s), (dollr_sign - s + i), to_replace);
        }
        dollr_sign = ft_strchr(dollr_sign + 1, '$');
    }
    return (s);
}