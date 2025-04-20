#include "minishell.h"

t_list *garbage_collector = NULL;

void print_strings(char **strs)
{
    int i = 0;
    while (strs[i])
    {
        printf("%s\n", strs[i]);
        i++;
    }
}

void print_node(t_tree *node) {
    if (node == NULL) {
        printf("Node is NULL\n");
        return;
    }
    printf("Node type: %d\n", node->type);
    printf("Node path: %s\n", node->path);
    printf("Node data: %s\n", node->data);

    if (node->args != NULL) {
        printf("Node args:\n");
        for (char **arg = node->args; *arg != NULL; arg++) {
            printf("  %s\n", *arg);
        }
    } else {
        printf("Node args: NULL\n");
    }

    printf("Node heredoc_content: %s\n", node->heredoc_content);

    if (node->left != NULL) {
        printf("Left child:\n");
        print_node(node->left);
    } else {
        printf("Left child: NULL\n");
    }

    if (node->right != NULL) {
        printf("Right child:\n");
        print_node(node->right);
    } else {
        printf("Right child: NULL\n");
    }

    if (node->parent != NULL) {
        printf("Parent node path: %s\n", node->parent->path);
        printf("Parent node data: %s\n", node->parent->data);
    } else {
        printf("Parent: NULL\n");
    }
}

int *get_exit_status(void)
{
    static int exit_status;
    return &exit_status;
}

char **get_envp(char **envp)
{
    static char **saved_envp;
    
    if (envp != NULL) {
        saved_envp = envp;
    }
    return saved_envp;
}

t_list *initialize_env_list(char **envp)
{
    static t_list *head;
    if (envp != NULL)
        head = strings_to_list(envp);
    return head;
}

void initialize_shell(t_list **env_list, char **envp)
{
    get_envp(envp); 
    *env_list = initialize_env_list(envp); 
}
void execute_tree(t_tree *tree)
{
    if (!tree)
        return;

    process_all_heredocs(tree);
    *get_exit_status() = execute_ast(tree);
}

int main(int ac, char **av, char **envp)
{
    (void)ac;
    (void)av;
    t_tree *tree;
    t_list *env_list;

    ft_set_interactive_signals();
    initialize_shell(&env_list, envp);
    
    while (1) {
        char *phrase;
        phrase = readline("$>");
        if (!phrase)
            ctrl_d_handle();
        char **commands = ft_split(phrase , "\n");
        while(commands && *commands)
        {
            tree = ilyas_parsing(*commands , env_list);
            //tree = parse_input(env_list);
            if (tree)
                execute_tree(tree);
            commands++;
        }
    }
}
