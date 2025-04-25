#include "minishell.h"

t_list *garbage_collector = NULL;

int *get_exit_status(void)
{
    static int exit_status;
    return &exit_status;
}

// Store and retrieve the original envp for execve
char **get_envp(char **envp)
{
    static char **saved_envp;
    if (envp)
        saved_envp = envp;
    return saved_envp;
}

t_list *initialize_env_list(char **envp)
{
    static t_list *head;
    if (envp != NULL)
        head = strings_to_list(envp);
    return head;
}

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

int execute_node(t_tree *node)
{
    if (!node)
        return 0;

    int status = 0;
    switch (node->type) {
    case COMMAND:
    {
        if (node->redirects)
            apply_redirections(node->redirects);
        status = fork_and_exec(node);
        if (node->redirects)
            restore_redirections(node->redirects);
        return status;
    }   
    case PIPE: {
        if (pipe(node->pipe_fds) < 0) {
            perror("pipe");
            return 1;
        }
        pid_t l = fork();
        if (l == 0) {
            dup2(node->pipe_fds[1], STDOUT_FILENO);
            close(node->pipe_fds[0]); close(node->pipe_fds[1]);
            execute_node(node->left);
            exit(0);
        }
        pid_t r = fork();
        if (r == 0) {
            dup2(node->pipe_fds[0], STDIN_FILENO);
            close(node->pipe_fds[1]); close(node->pipe_fds[0]);
            execute_node(node->right);
            exit(0);
        }
        close(node->pipe_fds[0]); close(node->pipe_fds[1]);
        waitpid(l, NULL, 0);
        waitpid(r, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
    case AND:
        status = execute_node(node->left);
        return (status == 0) ? execute_node(node->right) : status;
    case OR:
        status = execute_node(node->left);
        return (status != 0) ? execute_node(node->right) : status;
    case INPUT_REDIRECTION:
    case OUTPUT_REDIRECTION:
    case APP_OUTPUT_REDIRECTION:
    case APP_INPUT_REDIRECTION:
        return execute_node(node->left);
    default:
        return 0;
    }
}

void ft_free_split(char **split)
{
    if (!split)
        return;
        
    for (int i = 0; split[i]; i++)
        free(split[i]);
    
    free(split);
}

void print_node(t_tree *node, int depth)
{
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("[type=%d] data='%s' path='%s'\n", node->type, node->data ? node->data : "(null)", node->path ? node->path : "(null)");
    if (node->args && node->args[0]) {
        for (int i = 0; node->args[i]; i++) {
            for (int j = 0; j < depth + 1; j++) printf("  ");
            printf("arg[%d]='%s'\n", i, node->args[i]);
        }
    }
    if (node->redirects && node->redirects->file) {
        for (int i = 0; i < depth + 1; i++) printf("  ");
        printf("redirect: type=%d file='%s'\n", node->redirects->type, node->redirects->file);
    }
    print_node(node->left, depth + 1);
    print_node(node->right, depth + 1);
}

void attach_all_redirections(t_tree *node) {
    if (!node) return;
    
   if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION || 
        node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION) {
        
        t_redirection *redir_list = NULL;
        t_tree *cmd_node = extract_redirections(node, &redir_list);
        
        // If we found a command node at the bottom of the redirection chain
        if (cmd_node && cmd_node->type == COMMAND) {
            cmd_node->redirects = redir_list;
            printf("Command '%s' redirections:\n", cmd_node->data ? cmd_node->data : "(null)");
            t_redirection *temp = redir_list;
            while (temp) {
                printf("  Type: %d, File: %s\n", temp->type, temp->file ? temp->file : "(null)");
                temp = temp->next;
            }
        }
    }
    else {
        attach_all_redirections(node->left);
        attach_all_redirections(node->right);
    }
}

int main(int ac, char **av, char **envp)
{
    (void)ac; (void)av;
    t_tree *tree;
    t_list *env_list;

    ft_set_interactive_signals();
    // Save envp for execve
    get_envp(envp);
    // Build your linked-list of env vars
    env_list = initialize_env_list(envp);

    while (1) {
        char *input = readline("$> ");
        if (!input) ctrl_d_handle();
        if (*input) add_history(input);
        char **cmds = ft_split(input, "\n");
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stdin = dup(STDIN_FILENO);
        for (int i = 0; cmds && cmds[i]; i++) {
            tree = ilyas_parsing(cmds[i], env_list);
            if (tree) 
            {
                attach_all_redirections(tree);
                execute_node(tree);
            }
        }
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
    }
    return 0;
}
