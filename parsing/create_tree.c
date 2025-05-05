#include <minishell.h>

t_tree *create_one_node(char *command)
{
    t_tree *node = ft_malloc(sizeof(t_tree));
    node->path = NULL;
    node->data = command;
    node->args = NULL;
    node->heredoc_content = NULL;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->ops = NULL;
    node->type = COMMAND;
    node->redirects = NULL;
    node->to_skip = 0;
    node->pipe_fds[0] = -1;
    node->pipe_fds[1] = -1;
    return (node);
}

static t_tree *create_right_node(char **commands_files, int index, t_tree *parent)
{
    t_tree *node_assign;

    node_assign = ft_malloc(sizeof(t_tree));
    if (!commands_files[index])
        node_assign = NULL;
    node_assign->parent = parent;
    node_assign->data = commands_files[index];
    node_assign->type = (is_file((parent)->type) ? FT_FILE : COMMAND);
    if ((parent)->type == APP_INPUT_REDIRECTION)
        node_assign->type = FT_EOF;
    node_assign->left = NULL;
    node_assign->right = NULL;
    node_assign->heredoc_content = NULL;
    node_assign->args = NULL;
    node_assign->path = NULL;
    node_assign->ops = NULL;
    node_assign->to_skip = 0;
    node_assign->redirects = NULL;
    node_assign->pipe_fds[0] = -1;
    node_assign->pipe_fds[1] = -1;

    return (node_assign);
}
static t_tree *create_left_node(char **commands_files, int index)
{
    t_tree *node;

    node = ft_malloc(sizeof(t_tree));
    node->data = commands_files[index];
    node->type = COMMAND;
    node->heredoc_content = NULL;
    node->args = NULL;
    node->path = NULL;
    node->to_skip = 0;
    node->left = NULL;
    node->right = NULL;
    node->ops = NULL;
    node->redirects = NULL;
    node->pipe_fds[0] = -1;
    node->pipe_fds[1] = -1;
    node->parent = NULL;
    return (node);
}

void put_to_tree(t_tree **node, char **commands_files, int index, t_tree *last_node_parent)
{
    t_tree *node_assign;
    int index_shift;

    index_shift = 1;
    if (index < 0)
        return;
    if ((*node) != NULL)
    {
        if ((*node)->to_skip == 0)
        {
            (*node)->right = create_right_node(commands_files, index, *node);
            put_to_tree(&((*node)->left), commands_files, index - 1, *node);
        }
        else
            put_to_tree(&((*node)->left), commands_files, index, *node);
    }
    if ((*node) == NULL)
    {
        (*node) = create_left_node(commands_files, index);
        (*node)->parent = last_node_parent;
        return;
    }
}

t_tree *make_tree(char ***data, int *to_skip)
{
    char **ops;
    t_tree *tree;
    t_tree *head;
    int last_word;
    t_tree *parent;

    last_word = double_char_size(data[1]) - 1;
    if (last_word == -1)
        return (create_one_node(*data[0]));
    tree = ft_malloc(sizeof(t_tree));
    head = tree;
    parent = NULL;
    ops = data[1];
    while (last_word >= 0)
    {
        tree->redirects = NULL;
        tree->pipe_fds[0] = -1;
        tree->pipe_fds[1] = -1;
        tree->parent = parent;
        parent = tree;
        tree->to_skip = to_skip[last_word];
        tree->data = ops[last_word--];
        tree->type = get_data_type(tree->data);
        tree->right = NULL;
        tree->heredoc_content = NULL;
        tree->args = NULL;
        tree->path = NULL;
        tree->ops = NULL;
        tree->heredoc_content = NULL;
        
        if (last_word != -1)
        {
            tree->left = ft_malloc(sizeof(t_tree));
            tree = tree->left;
        }
    }
    tree->left = NULL;
    put_to_tree(&head, data[0], double_char_size(data[0]) - 1, tree);
    return (head);
}
