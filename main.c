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

int main(int ac, char **av, char **envp)
{
    t_tree *tree;
    char **paths;
    int status = -2;
    while (1)
    {
        get_envp(envp);
        paths = extract_paths(envp);
        tree = ilyas_parsing(1);
        split_tree(tree);
        add_paths_to_tree(tree, paths);
        if (builtins_engine(tree) != -1)
            continue;
        // process_heredocs(tree);   // dyali hadchi 
        status = execute_ast(tree); // syali hadchi
        // print_node(tree);
        printf("The Exit Status :: %d\n", status);
    }
    ft_free(garbage_collector);
}
