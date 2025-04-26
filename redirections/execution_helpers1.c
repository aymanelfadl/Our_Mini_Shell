#include "minishell.h"

static int handle_no_path(t_tree *node)
{
	fprintf(stderr, "minishell: %s: command not found\n", node->args[0]);
	*get_exit_status() = 127;
	return 127;
}

static int exec_child(t_tree *node)
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

static int handle_builtin(t_tree *node)
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
