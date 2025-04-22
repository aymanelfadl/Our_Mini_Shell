#include <minishell.h>


void split_tree(t_tree *tree)
{
    if (tree == NULL)
        return;
    if (tree->type == COMMAND)
        tree->data = handle_commandes_quoets(tree);
    if (!(tree->type == FT_FILE))
        tree->args = ft_split(tree->data, " \t");
    if (tree->type == FT_FILE)
        tree->args = ft_split_files(tree->data);
    split_tree(tree->right);
    split_tree(tree->left);
}

void add_paths_to_tree(t_tree *tree, char **paths)
{
    if (tree == NULL)
        return;
    if (tree->type == COMMAND)
    {
        tree->path = check_paths(paths, tree->args);
    }
    add_paths_to_tree(tree->right, paths);
    add_paths_to_tree(tree->left, paths);
}