#include "minishell.h"


void print_double_pointer(char **s)
{
    if (s == NULL)
        printf("double pointer is NULL\n");
    while (s && *s)
    {
        printf("%s", *s);
        s++;
        if (*s)
            printf(",");
    }
    printf("\n");
}
void print_tree(t_tree *tree)
{
    if (tree == NULL)
        return;
    print_tree(tree->left);
    printf("%s %d        double :", tree->data, tree->type);
    print_double_pointer(tree->args);
    if (tree->type == COMMAND)
        printf("      path : %s", tree->path);
    printf("\nnext\n");
    print_tree(tree->right);
}
int *get_exit_status(void)
{
    static int exit_status;
    return &exit_status;
}

void restore_std_fds(int saved_stdout, int saved_stdin)
{
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdout);
    close(saved_stdin);
}

void save_std_fds(int *saved_stdout, int *saved_stdin)
{
    *saved_stdout = dup(STDOUT_FILENO);
    *saved_stdin = dup(STDIN_FILENO);       
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
        }
        i++;
    }
    cleanup_heredoc_fds(tree);
}
