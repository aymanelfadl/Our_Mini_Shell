#include "minishell.h"

int ft_pwd(void)
{
    char buffer[1024];
    printf("%s\n", getcwd(buffer , 1000));
    return (0);
}