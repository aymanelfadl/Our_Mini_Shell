#include "minishell.h"

int execute_pipe(t_tree *node, int input_fd)
{
    int pipe_fd[2];
    pid_t pid;

    // Base case: node is a COMMAND (last in pipeline)
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

    // node is PIPE: create pipe and handle left & right
    if (pipe(pipe_fd) == -1)
        return (perror("pipe"), -1);

    // Left child: write to pipe
    pid = fork();
    if (pid == 0)
    {
        dup2(input_fd, STDIN_FILENO);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        if (input_fd != STDIN_FILENO)
            close(input_fd);
        execute_command_or_builtin(node->left);
        exit(0);
    }

    // Parent: close write, continue with right child
    close(pipe_fd[1]);
    if (input_fd != STDIN_FILENO)
        close(input_fd);

    // Recurse on right
    return execute_pipe(node->right, pipe_fd[0]);
}
