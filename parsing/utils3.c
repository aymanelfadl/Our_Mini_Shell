#include "minishell.h"

int ft_strcmp(const char *str1, const char *str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}


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