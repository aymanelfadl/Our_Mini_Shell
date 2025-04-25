#include "minishell.h"

int run_external_cmd(t_tree *node)
{
    pid_t pid;
    if (!node->path)
    {
        fprintf(stderr, "minishell: %s: command not found\n", node->args[0]);
        *get_exit_status() = 127;
        return 127;
    }
    pid = fork();
    if (pid < 0)
        return (perror("fork"), 1);
    if (pid == 0)
    {
        execve(node->path, node->args, list_to_char_array(initialize_env_list(NULL)));
        perror(node->args[0]);
        exit(127);
    }
    return wait_for_child(pid);
}
int fork_and_exec(t_tree *node)
{
    if (node->args && node->args[0])
    {
        int builtin_result = builtins_engine(node, initialize_env_list(NULL));
        if (builtin_result != -1)
        {
            if (builtin_result == 0)
                return 0;
            else
                return 1;
        }
    }
    return run_external_cmd(node);
}

int execute_command(t_tree *node)
{
    int status;

    if (node->redirects)
        if (apply_redirections(node->redirects))
            return 1;
    status = fork_and_exec(node);
    if (node->redirects)
        restore_redirections(node->redirects);
    return status;
}

int execute_and(t_tree *node)
{
    if (node->left->type == COMMAND)
        *get_exit_status() = execute_node(node->left);
    if (!*get_exit_status())
        return execute_node(node->right);
    return *get_exit_status();
}

int execute_or(t_tree *node)
{
    int status;

    status = execute_node(node->left);
    if (status)
        return execute_node(node->right);
    return status;
}
int execute_node(t_tree *node)
{
    int status;

    if (!node)
        return 0;
    status = 0;
    if (node->type == COMMAND)
        return execute_command(node);
    else if (node->type == PIPE)
        return execute_pipe(node);
    else if (node->type == AND)
        return execute_and(node);
    else if (node->type == OR)
        return execute_or(node);
    else if (node->type == INPUT_REDIRECTION ||
               node->type == OUTPUT_REDIRECTION ||
               node->type == APP_OUTPUT_REDIRECTION ||
               node->type == APP_INPUT_REDIRECTION)
        return execute_node(node->left);
    else 
        return 0;
}
