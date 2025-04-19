#include "minishell.h"

int execute_ast(t_tree *node)
{
    if (!node)
        return 0;

    if (node->type == COMMAND)
        return execute_command_or_builtin(node);
    else if (node->type == PIPE)
        return execute_pipe(node);
    else if (node->type == AND || node->type == OR)
        return execute_logical_operators(node);
    else
        return execute_redirection(node);
}

int execute_command_or_builtin(t_tree *node)
{
    t_list *envp = strings_to_list(get_envp(NULL));

    int builtin_status = builtins_engine(node, envp);
    if (builtin_status != -1)
        return builtin_status;

    return execute_external_command(node);
}

int execute_external_command(t_tree *node)
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
        return (perror("exec_command"), -1);
    else
    {
        waitpid(pid, &status, 0);
        if (WIFSIGNALED(status))
        {
            *get_exit_status() = 128 + WTERMSIG(status);
            return *get_exit_status();
        }
        else if (WIFEXITED(status))
        {
            *get_exit_status() = WEXITSTATUS(status);
            return *get_exit_status();
        }
        else
            return -1;
    }
    return 0;
}

int execute_pipe(t_tree *node)
{
    if (!node || !node->left || !node->right)
    {
        fprintf(stderr, "Error: Invalid pipe node structure.\n");
        return (EXIT_FAILURE);
    }
    
    if (node->left && node->left->heredoc_content)
        return execute_with_pipe(node, node->left->heredoc_content);
    return execute_with_pipe(node, NULL);
}

int execute_redirection(t_tree *node)
{
    if (!node)
        return -1;

    if (node->type == OUTPUT_REDIRECTION)
        return execute_output_redirection(node);
    else if (node->type == APP_OUTPUT_REDIRECTION)
        return execute_append_output_redirection(node);
    else if (node->type == INPUT_REDIRECTION)
        return execute_input_redirection(node);
    else if (node->type == APP_INPUT_REDIRECTION)
        return execute_append_input_redirection(node);
    else
        return -1;
}