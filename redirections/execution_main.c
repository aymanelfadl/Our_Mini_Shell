#include "minishell.h"

int execute_node(t_tree *node)
{
	int status;

	if (!node)
		return 0;
	status = 0;
	if (node->type == COMMAND)
		return execute_command(node);
	else if (node->type == PIPE)
		return execute_pipe(node);
	else if (node->type == AND)
		return execute_and(node);
	else if (node->type == OR)
		return execute_or(node);
	else if (node->type == INPUT_REDIRECTION ||
	           node->type == OUTPUT_REDIRECTION ||
	           node->type == APP_OUTPUT_REDIRECTION ||
	           node->type == APP_INPUT_REDIRECTION)
		return execute_node(node->left);
	else 
		return 0;
}

int execute_command(t_tree *node);
int fork_and_exec(t_tree *node);
