#include <minishell.h>

char *skip_spaces(char *s)
{
    while (*s && (*s == ' ' || *s == '\t'))
        s++;
    return (s);
}
int is_n_option(char *n_option, int wanna_skip)
{
    int len;
    char *extract_n;

    len = 0;
    n_option = ft_strtrim(n_option, " ");
    if (!*n_option)
        return (0);
    if ((*n_option == 34) || (*n_option == 39))
    {
        len += 2;
        extract_n = ft_substr(n_option + 1, 0, ft_strchr(n_option + 1, *n_option) - n_option -1);
    }
    else if (wanna_skip)
        extract_n = ft_substr(n_option, 0, my_strchr(n_option, "\t \0") - n_option);
    else
        extract_n = n_option;
    if (*extract_n != '-')
        return (0);
    extract_n++;
    len++;
    while (*extract_n)
    {
        len++;
        if (*extract_n != 'n')
            return (0);
        extract_n++;
    }
    return (len);
}

char *skip_all_n_options(char *to_write)
{
    while (*to_write)
    {
        to_write = skip_spaces(to_write);
        if (is_n_option(to_write, 1))
            to_write += is_n_option(to_write, 1);
        else
            break;
    }
    return (to_write);
}

static void ft_echo_write(char *to_write, int n_option)
{
    int fd;
    int i_did_write;

    fd = 1;
    while (to_write && *to_write)
    {
        if (*to_write == 34 || *to_write == 39)
        {
            i_did_write = 1;
            ft_putstr_fd(ft_substr(to_write + 1, 0, ft_strchr(to_write + 1, *to_write) - (to_write + 1)), fd);
            to_write = ft_strchr(to_write + 1, *to_write) + 1;
        }
        else if (*to_write == ' ' || *to_write == '\t')
        {
            to_write = skip_spaces(to_write);
            if (*to_write && i_did_write)
                ft_putchar_fd(' ', fd);
        }
        else
        {
            while (*to_write && (*to_write != ' ' && *to_write != '\t' && *to_write != 34 && *to_write != 39))
            {
                ft_putchar_fd(*to_write, fd);
                to_write++;
            }
            i_did_write = 1;
        }
    }
    if (!n_option)
        ft_putchar_fd('\n', 1);
}

void ft_echo(t_tree *node)
{
    int n_option;
    char **check_n_option;
    char *to_write;
    to_write = NULL;
    if (ft_strnstr(node->data, "-n", ft_strlen(node->data)))
    {
        check_n_option = node->args;
        if (is_n_option(check_n_option[1], 0))
            n_option = 1;
        else
            n_option = 0;
    }
    else
        n_option = 0;
    to_write = node->data + 4;
    if (n_option)
        to_write = skip_all_n_options(to_write);
    ft_echo_write(to_write, n_option);
}
