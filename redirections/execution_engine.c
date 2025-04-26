#include "minishell.h"

static int	handle_no_path(t_tree *node)
{
	fprintf(stderr, "minishell: %s: command not found\n", node->args[0]);
	*get_exit_status() = 127;
	return 127;
}

static int	exec_child(t_tree *node)
{
	execve(node->path, node->args, list_to_char_array(initialize_env_list(NULL)));
	perror(node->args[0]);
	exit(127);
}

int run_external_cmd(t_tree *node)
{
	pid_t pid;

	if (!node->path)
		return handle_no_path(node);
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
		exec_child(node);
	return wait_for_child(pid);
}

static int	handle_builtin(t_tree *node)
{
	int builtin_result;

	builtin_result = builtins_engine(node, initialize_env_list(NULL));
	if (builtin_result != -1)
	{
		if (builtin_result == 0)
			return 0;
		else
			return 1;
	}
	return -1;
}

int fork_and_exec(t_tree *node)
{
    int res;
    
	if (node->args && node->args[0])
	{
		res = handle_builtin(node);
		if (res != -1)
			return res;
	}
	return run_external_cmd(node);
}

static int	try_apply_redir(t_tree *node)
{
	if (node->redirects)
		if (apply_redirections(node->redirects))
			return 1;
	return 0;
}

int execute_command(t_tree *node)
{
	int status;

	if (try_apply_redir(node))
		return 1;
	status = fork_and_exec(node);
	if (node->redirects)
		restore_redirections(node->redirects);
	return status;
}

static int	execute_and_right(t_tree *node, int status)
{
	*get_exit_status() = status;
	if (status == 0)
		return execute_node(node->right);
	return status;
}

int execute_and(t_tree *node)
{
	int status = execute_node(node->left);
	return execute_and_right(node, status);
}

static int	execute_or_right(t_tree *node, int status)
{
	if (status)
		return execute_node(node->right);
	return status;
}

int execute_or(t_tree *node)
{
	int status;

	status = execute_node(node->left);
	return execute_or_right(node, status);
}

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
