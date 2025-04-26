#include "minishell.h"

int execute_and(t_tree *node)
{
	*get_exit_status() = execute_node(node->left);
    if (!*get_exit_status())
	    return execute_node(node->right);
}

int execute_or(t_tree *node)
{
	*get_exit_status() = execute_node(node->left);
    if (*get_exit_status())
        return execute_node(node->right);
}
