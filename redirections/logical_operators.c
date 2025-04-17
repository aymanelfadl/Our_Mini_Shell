#include <minishell.h>

int execute_logical_operators(t_tree *node)
{
    int status;

    if (node->type == AND)
    {
        status = execute_ast(node->left);
        if (!status)
            return execute_ast(node->right);
        return status;
    }
    else
    {
        status = execute_ast(node->left);
        if (!status)
            return status;
        return execute_ast(node->right);
    }
    return -1;
}


