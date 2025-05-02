#include "minishell.h"

int execute_node(t_tree *node)
{
    if (!node)
        return 0;
        
    if (node->type == COMMAND)
        return execute_command(node);
    else if (node->type == PIPE)
        return execute_pipe(node);
    else if (node->type == INPUT_REDIRECTION ||
             node->type == OUTPUT_REDIRECTION ||
             node->type == APP_OUTPUT_REDIRECTION ||
             node->type == APP_INPUT_REDIRECTION)
    {
		if (node->redirects)
			return apply_redirections(node->redirects);
		else
			return execute_node(node->left);
    }
    else 
        return 0;
}

