#include "minishell.h"

void handle_sigs(int sig)
{
    (void)sig;
    if (sig == SIGINT)
    {
        rl_replace_line("", 0);
        write(1, "\n", 1);
        rl_on_new_line();
        rl_redisplay();
        *get_exit_status() = 130;
    }
    else if (sig == SIGQUIT)
    {
        rl_replace_line("", 0);
        write(1, "", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

void ft_handle_signals()
{
    if (signal(SIGINT, handle_sigs) == (sig_t) -1)
        printf("Sig Err");
    else if (signal(SIGQUIT, handle_sigs) == (sig_t) -1)
        printf("Sig Err");
}




