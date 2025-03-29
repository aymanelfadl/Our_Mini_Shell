#include <minishell.h>

int execute_and_operator(t_tree *node)
{
    int status;

    status = execute_ast(node->left);
    if (!status)
        return  execute_ast(node->right);
    else
        return status;
}

int execute_or_operator(t_tree *node)
{
    execute_ast(node->left);
    execute_ast(node->right);
}

int execute_logical_operators(t_tree *node)
{
    if (node->type == AND)
        return execute_and_operator(node);
    else
        return execute_or_operator(node);
}


