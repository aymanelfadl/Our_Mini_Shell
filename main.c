#include "minishell.h"

t_list *garbage_collector = NULL;

char **get_envp(char **envp)
{
    static char **saved_envp;
    
    if (envp != NULL) {
        saved_envp = envp;
    }
    return saved_envp;
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

int main_engine(t_tree *node)
{
    int status;

    status = builtins_engine(node);
    if (status != -1)
        return status;
    else 
        return execute_ast(node);
}

int main(int ac, char **av, char **envp)
{
    (void)ac; (void)av;
    char **paths;
    t_tree *tree;

    get_envp(envp);
    while (1)
    {
        ft_handle_signals();

        tree = ilyas_parsing(envp);
        if (!tree)
        {
            *get_exit_status() = 130; 
            continue;
        }
        // print_node(tree);

        *get_exit_status() = main_engine(tree);
        printf ("the exit status is :: %d\n", *get_exit_status());
    }
}
