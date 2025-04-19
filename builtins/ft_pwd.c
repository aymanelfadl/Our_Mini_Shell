#include "minishell.h"

char* ft_pwd(void)
{
    char * buffer = malloc (1000);
    //printf("%s\n",getcwd(buffer , 1000));
    getcwd(buffer , 1000);
    printf("%s\n",buffer);
    return (buffer);
}