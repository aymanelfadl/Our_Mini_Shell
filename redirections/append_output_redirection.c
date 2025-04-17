#include "minishell.h"

int execute_append_output_redirection(t_tree *node)
{
    pid_t child_pid;
    int file_fd = -1;
    int status;

    child_pid = fork();
    if (child_pid == 0)
    {
        create_redir_files_and_get_last(node, &file_fd, APP_OUTPUT_REDIRECTION);
        if (dup2(file_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(file_fd);
            exit(EXIT_FAILURE);
        }
        close(file_fd);
        while (node->left && node->left->type == APP_OUTPUT_REDIRECTION)
            node = node->left;
        exit(execute_ast(node->left));
    }
    else if (child_pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    return WEXITSTATUS(status);
}
