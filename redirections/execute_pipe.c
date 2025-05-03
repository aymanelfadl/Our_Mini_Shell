#include "minishell.h"

int execute_pipe(t_tree *node)
{
    if (pipe(node->pipe_fds) < 0)
        return (perror("pipe"), 1);
    pid_t l = fork();
    if (l == 0)
    {
        dup2(node->pipe_fds[1], STDOUT_FILENO);
        close(node->pipe_fds[0]);
        close(node->pipe_fds[1]);
        exit(execute_node(node->left));
    }
    pid_t r = fork();
    if (r == 0)
    {
        dup2(node->pipe_fds[0], STDIN_FILENO);
        close(node->pipe_fds[1]);
        close(node->pipe_fds[0]);
        if (!node->right)
            apply_redirections(node->redirects);
        else
            exit(execute_node(node->right));
    }
    close(node->pipe_fds[0]);
    close(node->pipe_fds[1]);
    waitpid(l, NULL, 0);
    return wait_for_child(r);
}   
