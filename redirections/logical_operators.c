#include <minishell.h>

static int execute_and(t_tree *node)
{
    int status = execute_ast(node->left);
    if (!status)
        return execute_ast(node->right);
    return status;
}

static int execute_or(t_tree *node)
{
    int status = execute_ast(node->left);
    if (!status)
        return status;
    return execute_ast(node->right);
}

int execute_logical_operators(t_tree *node)
{
    if (!node || !node->left || !node->right)
        return -1;

    if (node->type == AND)
        return execute_and(node);
    else if (node->type == OR)
        return execute_or(node);

    return -1;
}


