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