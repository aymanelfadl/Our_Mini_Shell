#include "minishell.h"

int execute_pipe(t_tree *node, int input_fd)
{
    int pipe_fd[2];
    pid_t pid;

    if (node->type == COMMAND)
    {
        pid = fork();
        if (pid == 0)
        {
            if (input_fd != STDIN_FILENO)
            {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }
            execute_command_or_builtin(node);
            exit(0);
        }
        if (input_fd != STDIN_FILENO)
            close(input_fd);
        return 0;
    }
    if (pipe(pipe_fd) == -1)
        return (perror("pipe"), -1);
    pid = fork();
    if (pid == 0) 
    {
        dup2(input_fd,  STDIN_FILENO);
        dup2(pipe_fd[1], STDOUT_FILENO);

        close(pipe_fd[0]);
        close(pipe_fd[1]);
        if (input_fd != STDIN_FILENO)
            close(input_fd);

        execute_pipe(node->left, STDIN_FILENO);
        exit(0);
    }
    close(pipe_fd[1]);
    if (input_fd != STDIN_FILENO)
        close(input_fd);

    return execute_pipe(node->right, pipe_fd[0]);
}
