#include "minishell.h"

void ctrl_d_handle()
{
    printf("exit\n");
    ft_free(garbage_collector);
    exit(0);
}

void sigint_handler(int sig)
{
    (void)sig;

    *get_exit_status() = 130;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void heredoc_sigint_handler(int sig)
{
    (void)sig;
    
    *get_exit_status() = 130;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);    
}