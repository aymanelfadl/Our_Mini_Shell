#include "minishell.h"

int *get_last_in(void)
{
    static int fd_in;
    return &fd_in;
}

int *get_last_out(void)
{
    static int fd_out;
    return &fd_out;
}


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
    if (!cmd->parent)
    {
        pid_t p = fork();
        if (p == 0)
        {
            execute_external_command(cmd);
        }
        *get_exit_status() = wait_for_child(p);
    }
    else
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

void execute_redirection_cmd(t_tree *cmd, int fd_in, int fd_out)
{
    int pid;
    pid = fork();
    if (pid == 0)
    {
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out , STDOUT_FILENO);
        close(fd_in);
        close(fd_out);
        execute_command(cmd);
    }
    *get_exit_status() = wait_for_child(pid);
}

void set_fd_in(t_tree *op)
{
    int fd;
    fd = open(op->right->args[0],  O_RDONLY);
    if (fd == -1)
    {
        perror(op->right->args[0]);
        *get_exit_status() = -1;
    }
    if (*get_last_in())
        close(*get_last_in());
    *get_last_in() = fd;
}

void set_fd_out(t_tree *op)
{
    int fd;
    if (op->type == OUTPUT_REDIRECTION)
    {
        fd = open(op->right->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);   
        if (fd == -1)
        {
            perror(op->right->args[0]);
            *get_exit_status() = -1;
        }
    }
    else if (op->type == APP_OUTPUT_REDIRECTION)
    {
        fd = open(op->right->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1)
        {
            perror(op->right->args[0]);
            *get_exit_status() = -1;
        }
    }
    if (*get_last_out())
        close(*get_last_out());
    *get_last_out() = fd;
}

t_tree *get_most_left_cmd(t_tree *op)
{
    while (op && op->left)
        op = op->left;
    return op;
}

int execute_sub_tree(t_tree *op)
{
    if (op->type == COMMAND)
    {
        execute_command(op);
    }
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
    else if (op->type == OUTPUT_REDIRECTION || op->type == APP_OUTPUT_REDIRECTION)
    {
        set_fd_out(op);
        if (*get_exit_status() == -1)
            return 1;
        printf("out :: %d\n", *get_last_out());
        if (!op->parent || (op->parent->type != APP_OUTPUT_REDIRECTION && op->parent->type != OUTPUT_REDIRECTION))
            execute_redirection_cmd(get_most_left_cmd(op), *get_last_in(), *get_last_out());
    }
    else if (op->type == INPUT_REDIRECTION)
    {
        set_fd_in(op);
        if (*get_exit_status() == -1)
            return 1;
        if (!op->parent && (op->parent->type != APP_OUTPUT_REDIRECTION 
             && op->parent->type != OUTPUT_REDIRECTION && op->parent->type != INPUT_REDIRECTION))
            execute_redirection_cmd(get_most_left_cmd(op), *get_last_in(), *get_last_out());
    }
    return 0;
}

int execute_ast(t_tree *node)
{
    t_tree *mst_lft_op;
    int err = 0;
    mst_lft_op = find_most_left_op(node);
    while (mst_lft_op)
    {
        err = execute_sub_tree(mst_lft_op);
        if (err == 1)
            return err;
        mst_lft_op = mst_lft_op->parent;
    }
    return 0;
} 