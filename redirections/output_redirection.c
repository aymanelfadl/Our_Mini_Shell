#include "minishell.h"

int is_first_call(void *arg)
{
    if (!arg)
        return 1;
    return 0;
}

int execute_output_redirection(t_tree *node)
{
    pid_t child_pid;
    int file_fd;
    int status;
    static int first_fd;
    static char *file_name;
    static int fail;

    child_pid = fork();
    if (child_pid == 0)
    {
        if (is_first_call(NULL))
        {
            first_fd = open(node->right->data, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (first_fd == -1)
            {
                file_name = node->right->data;
                fail = 1;
            }
        }
        else
        {
            file_fd = open(node->right->data, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (file_fd == -1)
            {
                perror(node->right->data);
                exit(EXIT_FAILURE);
            }
            close(file_fd);
        }
        if (node->left->type != OUTPUT_REDIRECTION)
        {
            if (fail)
            {
                perror(file_name);
                exit(EXIT_FAILURE);
            }
            if (dup2(first_fd, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                exit(EXIT_FAILURE); 
            }
            close(first_fd);
        }
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
