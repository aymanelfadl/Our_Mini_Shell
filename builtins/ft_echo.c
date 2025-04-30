#include <minishell.h>


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
static char *ft_echo_write_helper(char *to_write, int *i_did_write)
{
    if (*to_write == 34 || *to_write == 39)
    {
        *i_did_write = 1;
        ft_putstr_fd(ft_substr(to_write + 1, 0, ft_strchr(to_write + 1, *to_write) - (to_write + 1)), 1);
        to_write = ft_strchr(to_write + 1, *to_write) + 1;
    }
    else if (*to_write == ' ' || *to_write == '\t')
    {
        to_write = skip_spaces(to_write);
        if (*to_write && *i_did_write)
            ft_putchar_fd(' ', 1);
    }
    else
    {
        while (*to_write && (*to_write != ' ' && *to_write != '\t' && *to_write != 34 && *to_write != 39))
        {
            ft_putchar_fd(*to_write, 1);
            to_write++;
        }
        *i_did_write = 1;
    }
    return (to_write);
}

static void ft_echo_write(char *to_write, int n_option)
{
    int i_did_write;
    i_did_write = 0;
    while (to_write && *to_write)
    {
        to_write = ft_echo_write_helper(to_write, &i_did_write);
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
