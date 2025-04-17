#include "minishell.h"

int execute_input_redirection(t_tree *node)
{
    pid_t child_pid;
    int file_fd;
    int status;

    child_pid = fork();
    if (child_pid == 0)
    {
        file_fd = open(node->right->data, O_RDONLY);
        if (file_fd == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);  
        }
        if (dup2(file_fd, STDIN_FILENO) == -1)
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
