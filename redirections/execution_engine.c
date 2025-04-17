#include "minishell.h"

int execute_ast(t_tree *node)
{
    if (node == NULL)
        return 0;
    if (node->type == COMMAND)
        return execute_command(node);
    else if (node->type == PIPE)
        return execute_pipe(node);
    else if ((node->type == AND) || (node->type == OR))
        return execute_logical_operators(node);
    else
        return execute_redirection(node);
    return 0;
}

int execute_pipe(t_tree *node)
{
    if (!node->left || !node->right)
        return -1;

    if (node->left->type == APP_INPUT_REDIRECTION || node->right->type == APP_INPUT_REDIRECTION) {
        return execute_heredoc_pipe(node->left);
    } else {
        return execute_simple_pipe(node);
    }
}

int execute_command(t_tree *node)
{
    pid_t pid;
    int status;
    
    pid = fork();
    if (pid == 0)
    {
        if ((node->path == NULL) || (execve(node->path, node->args, get_envp(NULL)) == -1)) 
        {
            write(2, node->args[0], strlen(node->args[0]));
            write(2, ": command not found\n", 21);
            exit(127);
        }
    }
    else if (pid < 0)
        return (perror("exec_command"),-1);
    else
    {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        else
            return -1;
    }
    return 0;
}

int execute_redirection(t_tree *node)
{
    if (node->type == OUTPUT_REDIRECTION || node->type == APP_OUTPUT_REDIRECTION)
    {
        if (node->type == OUTPUT_REDIRECTION)
            return execute_output_redirection(node);
        else
            return execute_append_output_redirection(node);
    }
    else if (node->type == INPUT_REDIRECTION)
        return execute_input_redirection(node);
    else if (node->type == APP_INPUT_REDIRECTION)
        return execute_append_input_redirection(node);
    return -1;
}

