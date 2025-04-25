#include "minishell.h"

// Fork and exec a simple COMMAND node
static int fork_and_exec(t_tree *node)
{
    int status;
    t_list *env_list = initialize_env_list(NULL);
    
    // Handle builtin commands
    if (node->args && node->args[0]) {
        int saved_stdin = -1, saved_stdout = -1;
        if (node->redirects) {
            saved_stdin = dup(STDIN_FILENO);
            saved_stdout = dup(STDOUT_FILENO);
            apply_redirections(node->redirects);
        }
        int builtin_result = builtins_engine(node, env_list);
        if (saved_stdin != -1) {
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
        }
        if (saved_stdout != -1) {
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }
        if (node->redirects)
            restore_redirections(node->redirects);
        if (builtin_result != -1)
            return builtin_result == 0 ? 0 : 1;
    }
    
    // If not a builtin, handle as external command
    if (!node->path) {
        fprintf(stderr, "minishell: %s: command not found\n", node->args[0]);
        *get_exit_status() = 127;  // Command not found exit code
        return 127;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        // Child process
        apply_redirections(node->redirects);
        execve(node->path, node->args, get_envp(NULL));
        perror(node->args[0]);
        exit(127);  // Command not found
    }
    
    // Parent process
    status = wait_for_child(pid);
    if (node->redirects)
        restore_redirections(node->redirects);
    *get_exit_status() = status;
    return status;
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
    int status;

    status = execute_node(node->left);
    if (!status)
        return status;
    return execute_node(node->right);
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
