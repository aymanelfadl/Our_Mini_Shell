#include "minishell.h"

int wait_for_child(pid_t child_pid)
{
    int status;
    
    if (child_pid <= 0)
        return 1;
    
    ft_set_default_signals();
    
    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        return 1;
    }
    
    ft_set_interactive_signals();
    
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    
    return 1;
}