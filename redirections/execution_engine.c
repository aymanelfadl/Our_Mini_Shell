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
    int builtin_status = builtins_engine(node, initialize_env_list(NULL));
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
        ft_set_default_signals();
        if ((node->path == NULL) || (execve(node->path, node->args, list_to_char_array(initialize_env_list(NULL))) == -1))
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
        ft_set_wait_signals();
        waitpid(pid, &status, 0);
        ft_set_interactive_signals();
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