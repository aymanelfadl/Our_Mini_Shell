#include "minishell.h"

int execute_with_pipe(t_tree *node, char *heredoc_content)
{
    int pip_fd[2];
    pid_t left_pid, right_pid;
    int status;
    int heredoc_pipe_fd[2]; 
    int input_for_left = -1;

    if (heredoc_content)
    {
        if (pipe(heredoc_pipe_fd) == -1) {
            perror("pipe (heredoc)");
            return -1;
        }
        if (write(heredoc_pipe_fd[1], heredoc_content, ft_strlen(heredoc_content)) == -1) {
            perror("write (heredoc)");
            close(heredoc_pipe_fd[0]);
            close(heredoc_pipe_fd[1]);
            return -1;
        }
        close(heredoc_pipe_fd[1]);
        input_for_left = heredoc_pipe_fd[0];
    }
    if (pipe(pip_fd) == -1) {
        perror("pipe (main)");
        if (heredoc_content) 
            close(heredoc_pipe_fd[0]);
        return -1;
    }

    left_pid = fork_and_execute(node->left, input_for_left, pip_fd[1]);
    if (heredoc_content)
        close(heredoc_pipe_fd[0]);
    close(pip_fd[1]);

    if (left_pid == -1) {
        close(pip_fd[0]);
        return -1;
    }

    right_pid = fork_and_execute(node->right, pip_fd[0], -1); 
    close(pip_fd[0]);

    if (right_pid == -1) {
        waitpid(left_pid, &status, 0);
        return -1;
    }
    waitpid(left_pid, &status, 0);
    return wait_for_child(right_pid);
}
