#include <minishell.h>

void add_files_to_args(t_tree *node)
{
    int i;

    i = 1;
    if (double_char_size(node->args) > 1)
    {
        if (!node->parent->left)
            node->parent->left = create_one_node(NULL);

        while (node->args[i])
        {
            if (my_strchr(node->args[i], " \t"))
                node->parent->left->data = ft_strjoin(node->parent->left->data, "'");
            node->parent->left->data = ft_strjoin(node->parent->left->data, node->args[i]);
            if (my_strchr(node->args[i], " \t"))
                node->parent->left->data = ft_strjoin(node->parent->left->data, "'");
            if (node->args[i + 1])
                node->parent->left->data = ft_strjoin(node->parent->left->data, " ");
            i++;
        }
    }
}

void split_tree(t_tree *tree)
{
    if (tree == NULL)
        return;
    if (tree->type == FT_FILE || tree->type == COMMAND)
    {
        tree->data = handle_commandes_quoets(tree->data);
        tree->args = ft_split_files(tree->data);
        if (tree->type == FT_FILE)
            add_files_to_args(tree);
    }
    else if (tree->type == FT_EOF)
    {
        tree->data = ft_strtrim(tree->data, " \t");
        tree->args = ft_malloc(sizeof(char *) * 2);
        tree->args[0] = tree->data;
        tree->args[1] = NULL;
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
