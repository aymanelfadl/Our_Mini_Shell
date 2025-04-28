#include "minishell.h"

int wait_for_child(pid_t child_pid)
{
    int status;
    
    set_child_running(1);
    if (child_pid <= 0)
        return 1;
    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        return 1;
    }    
    if (WIFEXITED(status))
    {
        set_child_running(0);
        return WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        set_child_running(0);
        return 128 + WTERMSIG(status);
    }
    set_child_running(0);
    return 1;
}