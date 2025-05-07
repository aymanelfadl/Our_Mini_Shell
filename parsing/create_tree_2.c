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

    node_assign = create_one_node(NULL);
    if (!commands_files[index])
        node_assign = NULL;
    node_assign->parent = parent;
    node_assign->data = commands_files[index];
    if (is_file((parent)->type))
        node_assign->type = FT_FILE;
    else
    {
        if ((parent)->type == APP_INPUT_REDIRECTION)
            node_assign->type = FT_EOF;
        else 
            node_assign->type = COMMAND;
    }
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
