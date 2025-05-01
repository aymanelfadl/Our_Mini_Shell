#include "minishell.h"

static void get_pwd_from_env(t_list *env)
{
    char *pwd;

    pwd = get_value(env, "PWD");
    if (pwd)
    {
        printf("%s\n",pwd);
        *get_exit_status() = 0;
    }
    else
    {
        ft_putstr_fd("cant get env\n", 2);
        *get_exit_status() = 1;
    }
}

void ft_pwd(t_list *envp)
{
    char *buffer;

    buffer = NULL;
    buffer = getcwd(NULL, 0);
    if (buffer)
    {
        printf("here\n");
        printf("%s\n", buffer);
        *get_exit_status() = 0;
    }
    else
        get_pwd_from_env(envp);
}