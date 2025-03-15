#include "minishell.h"

void split_tree(t_tree *tree)
{
    if (tree == NULL)
        return ;
    tree->args = ft_split(tree->data , " \t");
    split_tree(tree->right);
    split_tree(tree->left);
}
void print_tree(t_tree *tree)
{
    if (tree == NULL)
        return;
    printf("%s %d        double : ", tree->data, tree->type);
    print_double_pointer(tree->args);
    if (tree->type == COMMAND)
        printf("      path : %s",tree->path);
    printf("\nnext\n");
    // print_tree(tree->left);
    print_tree(tree->right);
}

void add_paths_to_tree(t_tree * tree , char **paths)
{
    if (tree == NULL)
        return ;
    if (tree->type == COMMAND)
        tree->path = check_paths(paths , tree->args);
    add_paths_to_tree(tree->right , paths);
    add_paths_to_tree(tree->left , paths);
}