#include "minishell.h"

void handle_sigint_prompt(int sig)
{
    (void)sig;

    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line(); 
    rl_replace_line("", 0);
    rl_redisplay();
    *get_exit_status() = 130;
}

// void sigint_handler_heredoc(int sig)
// {
//     (void)sig;

//     *heredoc_interrupted() = 1;
//     write(1, "\n", 1);
//     rl_done = 1;
//     rl_on_new_line();
// }


void ft_set_interactive_signals()
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    sa_int.sa_handler = handle_sigint_prompt;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART; 
    if (sigaction(SIGINT, &sa_int, NULL) == -1)
        perror("sigaction SIGINT interactive error");


    sa_quit.sa_handler = SIG_IGN; 
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
        perror("sigaction SIGQUIT interactive error");
}

void ft_set_wait_signals()
{
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
        perror("sigaction SIGINT ignore error");
    if (sigaction(SIGQUIT, &sa, NULL) == -1)
        perror("sigaction SIGQUIT ignore error");
}

// void ft_set_heredoc_signals()
// {
//     struct sigaction sa;

//     sa.sa_handler = sigint_handler_heredoc;
//     sigemptyset(&sa.sa_mask);
//     sa.sa_flags = 0;

//     if (sigaction(SIGINT, &sa, NULL) == -1)
//         perror("sigaction SIGINT heredoc error");
//     if (sigaction(SIGQUIT, &sa, NULL) == -1)
//         perror("sigaction SIGQUIT heredoc error");
// }

void ft_set_default_signals()
{
    struct sigaction sa;

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
        perror("sigaction SIGINT default error");
    if (sigaction(SIGQUIT, &sa, NULL) == -1)
        perror("sigaction SIGQUIT default error");
}
