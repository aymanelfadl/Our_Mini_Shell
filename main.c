#include "minishell.h"  // contains e_type, t_tree, t_redirection, init, parsing, etc.

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

// Set up redirection nodes properly
static void setup_redirections(t_tree *node)
{
    // Skip if no node
    if (!node)
        return;
        
    // Check if this node is a redirection type
    if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION || 
        node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION) {
        
        // Skip if already has redirections set up
        if (node->redirects)
            return;
            
        // Create a new redirection structure
        t_redirection *redir = malloc(sizeof(t_redirection));
        if (!redir) return;
        
        // Set redirection type
        switch (node->type) {
            case INPUT_REDIRECTION:
                redir->type = REDIR_INPUT;
                redir->fd_src = STDIN_FILENO;
                break;
            case OUTPUT_REDIRECTION:
                redir->type = REDIR_OUTPUT;
                redir->fd_src = STDOUT_FILENO;
                break;
            case APP_OUTPUT_REDIRECTION:
                redir->type = REDIR_APPEND;
                redir->fd_src = STDOUT_FILENO;
                break;
            case APP_INPUT_REDIRECTION:
                redir->type = REDIR_HEREDOC;
                redir->fd_src = STDIN_FILENO;
                break;
            default:
                free(redir);
                return;
        }
        
        // Set file name from the right child if available
        if (node->right && node->right->args) {
            redir->file = strdup(node->right->args[0]);
            if (!redir->file) {
                free(redir);
                return;
            }
        } else {
            // If no valid file name, create an empty file name
            redir->file = strdup("");
            if (!redir->file) {
                free(redir);
                return;
            }
        }
        
        redir->fd_dst = -1;  // Will be set when applied
        redir->next = NULL;
        
        // Attach redirection to tree node
        node->redirects = redir;
        
        // If there's a left child command node, attach redirection to it as well
        if (node->left && node->left->type == COMMAND) {
            node->left->redirects = redir;
        }
    }
    
    // Process children recursively
    if (node->left) setup_redirections(node->left);
    if (node->right) setup_redirections(node->right);
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

// Recursively execute any AST node
int execute_node(t_tree *node)
{
    if (!node)
        return 0;
    
    // Set up redirections in the tree
    setup_redirections(node);
    
    int status = 0;
    switch (node->type) {
    case COMMAND:
        return fork_and_exec(node);

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

    case INPUT_REDIRECTION:
        apply_redirections(node->redirects);
        status = execute_node(node->left);
        restore_redirections(node->redirects);
        return status;

    case OUTPUT_REDIRECTION:
    case APP_OUTPUT_REDIRECTION:
        apply_redirections(node->redirects);
        status = execute_node(node->left);
        restore_redirections(node->redirects);
        return status;

    case APP_INPUT_REDIRECTION: {
        int pfd[2];
        if (pipe(pfd) < 0) return 1;
        write(pfd[1], node->heredoc_content, strlen(node->heredoc_content));
        close(pfd[1]);
        int saved = dup(STDIN_FILENO);
        dup2(pfd[0], STDIN_FILENO);
        close(pfd[0]);
        status = execute_node(node->left);
        dup2(saved, STDIN_FILENO);
        close(saved);
        return status;
    }

    case AND:
        status = execute_node(node->left);
        return (status == 0) ? execute_node(node->right) : status;

    case OR:
        status = execute_node(node->left);
        return (status != 0) ? execute_node(node->right) : status;

    default:
        return 0;
    }
}

// Recursively free a tree node and all its resources
void free_tree(t_tree *node)
{
    if (!node)
        return;
        
    // Free resources in this node
    if (node->path)
        free(node->path);
    if (node->data)
        free(node->data);
    if (node->heredoc_content)
        free(node->heredoc_content);
        
    // Free array of arguments
    ft_free_split(node->args);
    ft_free_split(node->ops);
    
    // Free redirections linked list
    t_redirection *curr = node->redirects;
    while (curr) {
        t_redirection *next = curr->next;
        if (curr->file)
            free(curr->file);
        free(curr);
        curr = next;
    }
    
    // Recursively free children
    free_tree(node->left);
    free_tree(node->right);
    
    // Free the node itself
    free(node);
}

// Free a NULL-terminated array of strings
void ft_free_split(char **split)
{
    if (!split)
        return;
        
    for (int i = 0; split[i]; i++)
        free(split[i]);
    
    free(split);
}

// Print the AST tree structure for debugging
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
        // free(input);
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stdin = dup(STDIN_FILENO);
        for (int i = 0; cmds && cmds[i]; i++) {
            tree = ilyas_parsing(cmds[i], env_list);
            if (tree) {
                print_node(tree, 0); // Debug: print AST
                t_redirection *redir_list = NULL;
                t_tree *exec_tree = extract_redirections(tree, &redir_list);
                apply_redirections(redir_list);
                execute_node(exec_tree);
                restore_redirections(redir_list);
                free_redirections(redir_list);
                // free_tree(exec_tree); // if you want to free AST
            }
            dup2(saved_stdout, STDOUT_FILENO);
            dup2(saved_stdin, STDIN_FILENO);
        }
        close(saved_stdout);
        close(saved_stdin);
        // ft_free_split(cmds); // if you want
    }
    return 0;
}
