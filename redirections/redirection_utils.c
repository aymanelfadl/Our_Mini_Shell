#include "minishell.h"

int create_pipe(int pip_fd[2])
{
    if (pipe(pip_fd) == -1)
    {
        perror("pipe");
        return -1;
    }
    return 0;
}

pid_t fork_and_execute(t_tree *node_to_exec, int input_fd, int output_fd)
{
    pid_t child_pid;
    int exit_status;

    child_pid = fork();
    if (child_pid == -1)
    {
        perror("fork");
        return -1;
    }

    if (child_pid == 0)
    {
        if (input_fd != -1) {
            if (dup2(input_fd, STDIN_FILENO) == -1)
            {
                perror("dup2 input");
                if (output_fd != -1) 
                    close(output_fd);
                close(input_fd);
                exit(EXIT_FAILURE);
            }
            close(input_fd);
        }

        if (output_fd != -1)
        {
            if (dup2(output_fd, STDOUT_FILENO) == -1)
            {
                perror("dup2 output");
                close(output_fd);
                exit(EXIT_FAILURE);
            }
            close(output_fd);
        }

        exit_status = execute_ast(node_to_exec);
        
        exit(exit_status);
    }
    return child_pid;
}

int wait_for_child(pid_t child_pid)
{
    int status;

    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        return -1;
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

void create_redir_files_and_get_last(t_tree *node, int *last_fd, e_type type)
{
    int fd;

    if (node->left && ( node->left->type == OUTPUT_REDIRECTION || node->left->type == APP_OUTPUT_REDIRECTION))
        create_redir_files_and_get_last(node->left, last_fd, type);
    if (type == OUTPUT_REDIRECTION)
        fd = open(node->right->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else if (type == APP_OUTPUT_REDIRECTION)
        fd = open(node->right->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror(node->right->args[0]);
        exit(EXIT_FAILURE);
    }
    if (*last_fd != -1)
        close(*last_fd);
    *last_fd = fd;
}
