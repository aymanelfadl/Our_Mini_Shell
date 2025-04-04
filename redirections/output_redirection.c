#include "minishell.h"

static char *get_file_name(t_tree *node)
{
    t_tree tmp_node;
    int file_fd;

    while (node->parent && node->parent->type == OUTPUT_REDIRECTION)
    {
        file_fd = open(node->parent->data, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (file_fd == -1)
        {
            perror(node->right->data);
            exit(EXIT_FAILURE);
        }
        node = node->parent;
    }
    
    return node->right->data;
}

int execute_output_redirection(t_tree *node)
{
    pid_t child_pid;
    int file_fd;
    int status;

    child_pid = fork();

    if (child_pid == 0)
    {

        file_fd = open(get_file_name(node), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (file_fd == -1)
        {
            perror(node->right->data);
            exit(EXIT_FAILURE);
        }
        if (dup2(file_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE); 
        }
        close(file_fd);
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
