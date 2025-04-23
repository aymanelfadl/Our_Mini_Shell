#include "minishell.h"

t_tree *find_most_left_op(t_tree *node)
{
    while (node && node->left && node->left->type != COMMAND)
        node = node->left;
    return node;
}

void execute_external_command(t_tree *node)
{
    if ((node->path == NULL) || (execve(node->path, node->args, list_to_char_array(initialize_env_list(NULL))) == -1))
    {
            write(2, node->args[0], strlen(node->args[0]));
            write(2, ": command not found\n", 21);
            exit(127);
    }
}
void execute_command(t_tree *cmd)
{
    int builtin_status = builtins_engine(cmd, initialize_env_list(NULL));
    if (builtin_status != -1)
        return ;

    execute_external_command(cmd);
}

void execute_pipe(t_tree *cmd)
{
    int p_fd[2];
    pid_t pid;

    if (cmd->type == COMMAND)
    {    
        if (pipe(p_fd) == -1)
            perror("pipe");
    
        pid = fork();
        if (pid == 0)
        {
            close(p_fd[0]);
            dup2(p_fd[1], STDOUT_FILENO);
            close(p_fd[1]);
            execute_command(cmd);
        }
        close(p_fd[1]);
        dup2(p_fd[0], STDIN_FILENO);
        close(p_fd[0]);
    }
}

int get_fd(t_tree *op)
{
    int fd;
    while (op)
    {
        if (op->type == OUTPUT_REDIRECTION)
            fd = open(op->right->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        else if (op->type == APP_OUTPUT_REDIRECTION)
            fd = open(op->right->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
        op = op->parent;
    }
    return fd;
} 

void execute_out_redirection(t_tree *cmd, int fd_out)
{
    int pid;
    if (cmd->type == COMMAND)
    {
        pid = fork();
        if (pid == 0)
        {
            dup2(fd_out , STDOUT_FILENO);
            close(fd_out);
            execute_command(cmd);
        }
    }
}

void execute_sub_tree(t_tree *op)
{
    if (op->type == PIPE)
    {
        execute_pipe(op->left);
        if (op->parent && op->parent->type == PIPE)
        {
            execute_pipe(op->right);
        }
        else
        {
            execute_command(op->right);
        }
    }
    else if (op->type == AND)
    {
        if (op->parent && op->parent->type == PIPE)
        {
            execute_pipe(op->right);
        }
        else
        {
            execute_command(op->right);
        }
    }
    else if (op->type == OUTPUT_REDIRECTION || op->type == APP_OUTPUT_REDIRECTION )
    {
        int fd_out = get_fd(op);
        execute_out_redirection(op->left, fd_out);
    }

}

int execute_ast(t_tree *node)
{
    t_tree *mst_lft_op;

    mst_lft_op = find_most_left_op(node);
    while (mst_lft_op)
    {
        execute_sub_tree(mst_lft_op);
        mst_lft_op = mst_lft_op->parent;
    }
    return 0;
} 