#include "minishell.h"

int *get_exit_status(void)
{
    static int exit_status;
    return &exit_status;
}

void close_std_fds(int *saved_stdout, int *saved_stdin)
{
    close(*saved_stdin);
    close(*saved_stdout);
}

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

void execute_commands(char **cmds, t_list *env_list)
{
    t_tree *tree;
    int i;

    tree = NULL;
    i = 0;
    while (cmds && cmds[i])
    {
        tree = ilyas_parsing(cmds[i], env_list);
        if (tree)
        {
            attach_all_redirections(tree);
            if (process_all_heredocs(tree) == -1)
            {
                cleanup_heredoc_fds(tree);
                *get_exit_status() = 130;
                break;
            }
            *get_exit_status() = execute_node(tree);
            cleanup_heredoc_fds(tree);
        }
        i++;
    }
    cleanup_heredoc_fds(tree);
}
