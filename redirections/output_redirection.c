#include "minishell.h"

void create_redir_files_and_get_last(t_tree *node, int *last_fd)
{
    int fd;

    if (node->left && node->left->type == OUTPUT_REDIRECTION)
        create_redir_files_and_get_last(node->left, last_fd);
    fd = open(node->right->data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror(node->right->data);
        exit(EXIT_FAILURE);
    }
    if (*last_fd != -1)
        close(*last_fd);
    *last_fd = fd;
}
int execute_output_redirection(t_tree *node)
{
    pid_t child_pid;
    int fd;
    int status;

    fd = -1;
    child_pid = fork();
    if (child_pid == 0)
    {
        create_redir_files_and_get_last(node, &fd);
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);
        while (node->left && node->left->type == OUTPUT_REDIRECTION)
            node = node->left;
        exit(execute_ast(node->left));
    }
    else if (child_pid == -1)
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        return EXIT_FAILURE;
    }
    return WEXITSTATUS(status);
}
