#include "minishell.h"

char* ft_pwd(void)
{
    char * buffer = malloc (1000);
    if (buffer && getcwd(buffer, 1000) != NULL) {
        printf("%s\n", buffer);
    } else {
        perror("pwd"); 
        free(buffer); 
        return NULL;
    }
    return (buffer);
}