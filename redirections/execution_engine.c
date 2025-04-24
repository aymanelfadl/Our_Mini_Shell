#include "minishell.h"

int *get_last_in(void)
{
    static int fd_in = 0;
    return &fd_in;
}

int *get_last_out(void)
{
    static int fd_out = 1;
    return &fd_out;
}

pid_t *get_last_pipe_pid(void)
{
    static pid_t pipe_pid = 0;
    return &pipe_pid;
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
    int status;
    pid_t pid_left;

    if (pipe(p_fd) == -1)
    {
        perror("pipe");
        *get_exit_status() = 1;
        return;
    }
    
    pid_left = fork();
    if (pid_left == 0)
    {
        close(p_fd[0]); 
        dup2(p_fd[1], STDOUT_FILENO);
        close(p_fd[1]);
        if (*get_last_in() != STDIN_FILENO)
        {
            dup2(*get_last_in(), STDIN_FILENO);
            close(*get_last_in());
        }
        execute_command(cmd);
        exit(0);
    }
    close(p_fd[1]); 
    dup2(p_fd[0], STDIN_FILENO);
    close(p_fd[0]);
    
    // Store the child PID for later collection
    // We don't wait here as this would block the pipeline,
    // but we'll collect the exit status later
    *get_last_pipe_pid() = pid_left;
}

void execute_redirection_cmd(t_tree *cmd, int fd_in, int fd_out)
{
    int pid;
    pid = fork();
    if (pid == 0)
    {
        if (fd_in != STDIN_FILENO)
        {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        
        if (fd_out != STDOUT_FILENO)
        {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        
        execute_command(cmd);
        exit(0);
    }
    
    *get_exit_status() = wait_for_child(pid);
    
    if (fd_in != STDIN_FILENO)
    {
        close(*get_last_in());
        *get_last_in() = STDIN_FILENO;
    }
}

void set_fd_in(t_tree *op)
{
    int fd;
    fd = open(op->right->args[0], O_RDONLY);
    if (fd == -1)
    {
        perror(op->right->args[0]);
        *get_exit_status() = 1; // Standard error code instead of -1
        return; 
    }
    if (*get_last_in() > 0)
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
            *get_exit_status() = 1; // Standard error code instead of -1
            return;
        }
    }
    else if (op->type == APP_OUTPUT_REDIRECTION)
    {
        fd = open(op->right->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1)
        {
            perror(op->right->args[0]);
            *get_exit_status() = 1; // Standard error code instead of -1
            return;
        }
    }
    if (*get_last_out() > 1)
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
        if (op->left->type == COMMAND)
            execute_pipe(op->left);
        if (op->parent && op->parent->type == PIPE)
        {
            execute_pipe(op->right);
        }
        else
        {
            pid_t pid = fork();
            if (pid == 0) execute_command(op->right);
            *get_exit_status() = wait_for_child(pid);       
            if (*get_last_pipe_pid() > 0)
            {
                int status;
                waitpid(*get_last_pipe_pid(), &status, 0);
                if (WIFEXITED(status))
                    *get_exit_status() = WEXITSTATUS(status);
                else if (WIFSIGNALED(status))
                    *get_exit_status() = 128 + WTERMSIG(status);
                *get_last_pipe_pid() = 0;
            }
        }
    }
    else if (op->type == AND)
    {
        if (*get_exit_status() == 0) 
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
    }
    else if (op->type == OR)
    {
        if (*get_exit_status() != 0) 
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
    }
    else if (op->type == OUTPUT_REDIRECTION || op->type == APP_OUTPUT_REDIRECTION)
    {
        set_fd_out(op);
        if (*get_exit_status() != 0) 
            return 1;
        if (!op->parent || (op->parent->type != APP_OUTPUT_REDIRECTION && op->parent->type != OUTPUT_REDIRECTION))
            execute_redirection_cmd(get_most_left_cmd(op), *get_last_in(), *get_last_out());
    }
    else if (op->type == INPUT_REDIRECTION)
    {
        *get_last_out() = STDOUT_FILENO;
        set_fd_in(op);
        if (*get_exit_status() != 0)
            return 1;
        if (!op->parent)
            execute_redirection_cmd(get_most_left_cmd(op), *get_last_in(), *get_last_out());
        if (op->parent && op->parent->type == PIPE)
            execute_pipe(get_most_left_cmd(op));
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
