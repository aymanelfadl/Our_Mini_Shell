#include "minishell.h"

static void exec_child(t_tree *node)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (execve(node->path, node->args, list_to_char_array(initialize_env_list(NULL))) == -1)
	{
		if (errno == ENOEXEC)
		{
			char *new_args[] = {"/bin/sh", node->path, NULL};
			execve("/bin/sh", new_args, list_to_char_array(initialize_env_list(NULL)));
		}
		else if (errno == EACCES)
		{
			ft_putstr_fd("minishell: ", 2);
			perror(node->args[0]);
			exit(126);
		}
		ft_putstr_fd("minishell: ", 2);
		perror(node->args[0]);	
		exit(127);
	}
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

static int run_external_cmd(t_tree *node)
{
	pid_t pid;

	if (!node->path)
		return handle_no_path(node);
	if (isdirectory(node->path))
		return (command_path_is_dir(node->path));
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
		exec_child(node);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	return wait_for_child(pid);
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
