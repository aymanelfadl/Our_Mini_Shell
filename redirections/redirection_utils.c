#include "minishell.h"

int wait_for_child(pid_t child_pid)
{
    int status;

    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        return -1;
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}
