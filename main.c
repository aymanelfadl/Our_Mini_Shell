#include "minishell.h"

t_list *garbage_collector = NULL;

int *get_exit_status(void)
{
    static int exit_status;
    return &exit_status;
}

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

int main(int ac, char **av, char **envp)
{
    (void)ac; (void)av;
    t_tree *tree;
    t_list *env_list;
    char *input;

    env_list = initialize_env_list(envp);
    
    setup_signals();
    
    while (1) {
        input = readline("$> ");
        if (!input) 
            ctrl_d_handle();
        if (input && *input)
        {

            add_history(input);
            
            char **cmds = ft_split(input, "\n");
            int saved_stdout = dup(STDOUT_FILENO);
            int saved_stdin = dup(STDIN_FILENO);
            
            for (int i = 0; cmds && cmds[i]; i++) {
                tree = ilyas_parsing(cmds[i], env_list);
                if (tree) {
                    attach_all_redirections(tree);
                    set_child_running(1);
                    *get_exit_status() = execute_node(tree);
                    set_child_running(0);
                }
            }
            
            dup2(saved_stdout, STDOUT_FILENO);
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdout);
            close(saved_stdin);
        }
    }
    return 0;
}