#include "minishell.h"

void handle_sigint(int sig)
{
    (void)sig;
    rl_replace_line("", 0);
    write(1, "\n", 1);            
    rl_on_new_line();
    rl_redisplay();
    *get_exit_status() = 130;
}

void ft_handle_signals()
{
    if (signal(SIGINT, handle_sigint) == (sig_t) -1)
        printf("Sig Err");
}




