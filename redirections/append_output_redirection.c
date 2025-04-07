#include "minishell.h"

static t_tree *get_cmd(t_tree *node)
{
    t_tree *current = node;
    
    while (current && current->left)
    {
        if (current->left->type == COMMAND)
            return current->left;
        current = current->left;
    }
    
    if (current && current->type == COMMAND)
        return current;
    
    return NULL;
}


int execute_append_output_redirection(t_tree *node)
{
    pid_t child_pid;
    int file_fd = -1;
    int status;

    child_pid = fork();
    if (child_pid == 0)
    {
        file_fd = open(node->right->data, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (file_fd == -1)
        {
            perror(node->right->data);
            exit(EXIT_FAILURE);
        }
        if (dup2(file_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(file_fd);
            exit(EXIT_FAILURE);
        }
        close(file_fd);
        execute_command(get_cmd(node));
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
