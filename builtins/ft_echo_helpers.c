#include <minishell.h>

char *skip_spaces(char *s)
{
    while (*s && (*s == ' ' || *s == '\t'))
        s++;
    return (s);
}
static char *get_extract_n(char *n_option, int wanna_skip, int *len)
{
    char *extract_n;
    if ((*n_option == 34) || (*n_option == 39))
    {
        (*len) = (*len) + 2;
        extract_n = ft_substr(n_option + 1, 0, ft_strchr(n_option + 1, *n_option) - n_option - 1);
    }
    else if (wanna_skip)
        extract_n = ft_substr(n_option, 0, my_strchr(n_option, "\t \0") - n_option);
    else
        extract_n = n_option;
    return (extract_n);
}
int is_n_option(char *n_option, int wanna_skip)
{
    int len;
    char *extract_n;

    len = 0;
    n_option = ft_strtrim(n_option, " ");
    if (!*n_option)
        return (0);
    extract_n = get_extract_n(n_option, wanna_skip, &len);
    if (*extract_n != '-')
        return (0);
    extract_n++;
    while (*extract_n)
    {
        if (++len && *extract_n != 'n')
            return (0);
        extract_n++;
    }
    return (++len);
}