#include <minishell.h>

t_tree *find_first_commamd_at_left(t_tree *tree)
{
    t_tree *command;
    if (tree->right && tree->right->type == COMMAND)
        return (tree->right);
    if ((tree->left) && tree->left->type == COMMAND)
        return (tree->left);
    return (find_first_commamd_at_left(tree->left));
}
void there_is_a_command(t_tree *tree, int *there_is_a_comm)
{
    if (tree == NULL)
        return;
    if (tree->type == COMMAND)
    {
        *there_is_a_comm = 1;
        return;
    }
    there_is_a_command(tree->left, there_is_a_comm);
    there_is_a_command(tree->left, there_is_a_comm);
    return;
}

void split_tree(t_tree *tree)
{
    char *original_eof;
    if (tree == NULL)
        return;
    tree->data = ft_strtrim(tree->data, " \t");
    if (tree->type == FT_FILE || tree->type == COMMAND)
    {
        tree->data = handle_commandes_quoets(tree->data);
        tree->args = ft_split_files(tree->data);
        if (tree->type == FT_FILE)
            add_files_to_args(tree);
    }
    else if (tree->type == FT_EOF)
    {
        original_eof = ft_strtrim(get_original_eof(tree->data), " \t");
        tree->data = handle_commandes_quoets(tree->data);
        tree->args = ft_split_files(tree->data);
        tree->args[0] = original_eof;
        add_files_to_args(tree);
    }
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
