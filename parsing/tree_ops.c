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
void add_files_to_args(t_tree *node)
{
    int i;
    t_tree *first_command_at_left;
    char *quotes_to_add;

    i = 1;
    if (double_char_size(node->args) > 1)
    {
        if (!node->parent->left)
            node->parent->left = create_one_node(NULL);

        while (node->args[i])
        {
            quotes_to_add = "'";
            if (ft_strchr(node->args[i], 39))
                quotes_to_add = "\x22";
            first_command_at_left = find_first_commamd_at_left(node->parent);
            first_command_at_left->data = ft_strjoin(first_command_at_left->data, " ");
            first_command_at_left->data = ft_strjoin(first_command_at_left->data, quotes_to_add);
            first_command_at_left->data = ft_strjoin(first_command_at_left->data, node->args[i]);
            first_command_at_left->data = ft_strjoin(first_command_at_left->data, quotes_to_add);
            i++;
        }
    }
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
        original_eof = ft_strtrim(get_original_eof(tree->data ), " \t");
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
