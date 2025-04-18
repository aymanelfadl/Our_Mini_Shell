#include "minishell.h"

static int execute_with_pipe(t_tree *node, char *heredoc_content)
{
    int pip_fd[2];
    pid_t left_pid, right_pid;
    int status;

    if (create_pipe(pip_fd) == -1)
        return -1;
    if (heredoc_content)
    {
        if (write(pip_fd[1], heredoc_content, ft_strlen(heredoc_content)) == -1)
        {
            perror("write");
            close(pip_fd[0]);
            close(pip_fd[1]);
            return -1;
        }
        close(pip_fd[1]);
    }
    left_pid = fork_and_execute(node, heredoc_content ? pip_fd[0] : -1, pip_fd[1], 1);
    close(pip_fd[0]);
    close(pip_fd[1]);
    if (left_pid == -1)
        return -1;
    if (!heredoc_content)
    {
        right_pid = fork_and_execute(node, pip_fd[0], -1, 0);
        if (right_pid == -1)
            return -1;
        return wait_for_child(right_pid);
    }
    return wait_for_child(left_pid);
}


int execute_simple_pipe(t_tree *node)
{
    if (!node->left || !node->right)
    {
        printf("Error: Invalid pipe node.\n");
        return -1;
    }
    return execute_with_pipe(node, NULL);
}

int execute_heredoc_pipe(t_tree *node)
{
    if (!node->left || !node->heredoc_content)
    {
        printf("Error: Invalid heredoc node.\n");
        return -1;
    }
    return execute_with_pipe(node, node->heredoc_content);
}
