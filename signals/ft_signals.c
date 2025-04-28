#include "minishell.h"

void ctrl_d_handle()
{
    printf("exit\n");
    ft_free(garbage_collector);
    exit(0);
}

