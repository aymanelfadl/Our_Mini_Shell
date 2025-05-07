#include "minishell.h"

int wait_for_child(pid_t child_pid)
{
    int status;
    
    if (child_pid <= 0)
        return 1;
    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        return 1;
    }    
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    return 1;
}

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
    	close_saved_fds();
        exit(execute_node(node->left));
    }
    pid_t r = fork();
    if (r == 0)
    {
        dup2(node->pipe_fds[0], STDIN_FILENO);
        close(node->pipe_fds[1]);
        close(node->pipe_fds[0]);
        if (!node->right)
        {
            apply_redirections(node->redirects);
        	close_saved_fds();
            exit(EXIT_SUCCESS);
        }
    	close_saved_fds();
        exit(execute_node(node->right));
    }
    close(node->pipe_fds[0]);
    close(node->pipe_fds[1]);
    waitpid(l, NULL, 0);
    return wait_for_child(r);
}   
