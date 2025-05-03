#include <minishell.h>

int commas_ops_check(char *s)
{
    int commas;
    if (!s)
        return (0);
    while (*s)
    {
        if (*s == 34 | *s == 39)
        {
            if (ft_strchr(s + 1, *s))
                s = ft_strchr(s + 1, *s) + 1;
            else
                return (0);
        }
        else
            s++;
    }
    return (1);
}

int extract_ops_helper(char *s, char **ops)
{
    int i;

    i = 0;
    while (ops && *ops)
    {
        if (my_strnstr(s, *ops, ft_strlen(*ops)))
            return (i);
        i++;
        ops++;
    }
    return (-1);
}

char **extract_ops(char *s)
{
    char *all_ops[8] = {">>", "<<", ">", "<", "||", "|", "&&", NULL};
    char **ops;
    int i;
    int op_found;
    int size;

    size = ops_size(s, all_ops);
    if (size == 0)
        return (NULL);
    i = 0;
    ops = ft_malloc((sizeof(char **) * size));
    while (*s)
    {
        if (extract_ops_helper(s, all_ops) >= 0)
        {
            ops[i++] = ft_strdup(all_ops[extract_ops_helper(s, all_ops)]);
            s += ft_strlen(all_ops[extract_ops_helper(s, all_ops)]);
        }
        else
            s++;
    }
    ops[i] = NULL;
    return (ops);
}

