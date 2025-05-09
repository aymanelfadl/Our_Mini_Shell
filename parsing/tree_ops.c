#include <minishell.h>

static void add_files_to_args_helper(t_tree *node, t_tree **first_command_at_left, int *j)
{
    int i;
    char *quotes_to_add;

    i = *j;

    if (first_command_at_left && *first_command_at_left)
    {
        quotes_to_add = "'";
        if (my_strchr(node->args[i], "'\x22"))
        {
            if (*my_strchr(node->args[i], "'\x22") == 39)
                quotes_to_add = "\x22";
        }
        if ((*first_command_at_left)->data)
            (*first_command_at_left)->data = ft_strjoin((*first_command_at_left)->data, " ");
        (*first_command_at_left)->data = ft_strjoin((*first_command_at_left)->data, node->args[i]);
    }
    i++;
    *j = i;
}

static t_tree *get_node_command(t_tree *to_search)
{
    t_tree *first_command_at_left;

    first_command_at_left = NULL;
    while (to_search && !first_command_at_left)
    {
        if (!to_search->right)
        {
            first_command_at_left = create_one_node(NULL);
            to_search->right = first_command_at_left;
            first_command_at_left->parent = to_search;
        }
        else if (to_search->right->type == COMMAND)
            first_command_at_left = to_search->right;
        else if (to_search->left == NULL)
        {
            first_command_at_left = create_one_node(NULL);
            to_search->left = first_command_at_left;
            first_command_at_left->parent = to_search;
        }
        else if (to_search->left->type == COMMAND)
            first_command_at_left = to_search->left;
        to_search = to_search->left;
    }
    return (first_command_at_left);
}
static void add_files_to_args(t_tree *node)
{
    int i;
    t_tree *first_command_at_left;
    char *quotes_to_add;

    i = 1;
    first_command_at_left = NULL;
    if (double_char_size(node->args) > 1)
    {
        first_command_at_left = get_node_command(node->parent);
        while (node->args[i] && first_command_at_left)
        {
            add_files_to_args_helper(node, &first_command_at_left, &i);
        }
        if (first_command_at_left)
            first_command_at_left->args = ft_split_files(first_command_at_left->data);
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
        original_eof = ft_strtrim(get_original_eof(tree->data), " \t");
        tree->data = handle_commandes_quoets(tree->data);
        tree->args = ft_split_files(tree->data);
        tree->args[0] = original_eof;
        add_files_to_args(tree);
    }
    split_tree(tree->left);
    split_tree(tree->right);
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
