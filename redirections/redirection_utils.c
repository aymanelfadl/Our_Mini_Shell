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

pid_t fork_and_execute(t_tree *node, int input_fd, int output_fd, int execute_left)
{
    int heredoc_pipe[2];

    if (node->heredoc_content)
    {
        if (pipe(heredoc_pipe) == -1)
        {
            perror("heredoc pipe");
            return -1;
        }

        write(heredoc_pipe[1], node->heredoc_content, strlen(node->heredoc_content));
        close(heredoc_pipe[1]);

        input_fd = heredoc_pipe[0];
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        if (input_fd != -1 && dup2(input_fd, STDIN_FILENO) == -1)
        {
            perror("dup2 input");
            exit(EXIT_FAILURE);
        }
        if (output_fd != -1 && dup2(output_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2 output");
            exit(EXIT_FAILURE);
        }

        if (input_fd != -1)
            close(input_fd);
        if (output_fd != -1)
            close(output_fd);

        if (execute_left)
            exit(execute_ast(node->left));
        else
            exit(execute_ast(node->right));
    }

    if (node->heredoc_content)
        close(heredoc_pipe[0]); 

    return pid;
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
        fd = open(node->right->data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else if (type == APP_OUTPUT_REDIRECTION)
        fd = open(node->right->data, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror(node->right->data);
        exit(EXIT_FAILURE);
    }
    if (*last_fd != -1)
        close(*last_fd);
    *last_fd = fd;
}
