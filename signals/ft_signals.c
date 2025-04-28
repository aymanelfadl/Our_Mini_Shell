#include "minishell.h"

void ctrl_d_handle()
{
    printf("exit\n");
    ft_free(garbage_collector);
    exit(0);
}

static int *child_running_ptr(void)
{
    static int child_running = 0;
    return &child_running;
}

/* Set child running state */
void set_child_running(int value)
{
    *child_running_ptr() = value;
}

/* Get child running state */
int get_child_running(void)
{
    return *child_running_ptr();
}

/* SIGINT handler (Ctrl+C) */
void sigint_handler(int sig)
{
    (void)sig;
    if (get_child_running())
        write(1, "\n", 1);
    else
    {
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
        *get_exit_status() = 130;
    }
}

/* SIGQUIT handler (Ctrl+\) */
void sigquit_handler(int sig)
{
    (void)sig;
    // Do nothing
}

/* Setup parent signals */
void setup_signals(void)
{
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
}

void reset_signals_in_child(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

