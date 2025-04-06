#include "minishell.h"

static int get_last_fd(t_tree *node)
{
    return open(node->right->data, O_CREAT | O_WRONLY | O_TRUNC, 0644);
}
static char *open_files(t_tree *node)
{
    static char *fail;
    int fd;
    fd = open(node->right->data, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        // @TODO 7ma9e khaliha mara akhera mhm 
        // you shoulkd satrt from doen to up 
    }
    close(fd);
    return fail
}

int execute_output_redirection(t_tree *node)
{
    pid_t child_pid;
    int status;
    static int last_fd;
    static char *fail;

    if (!last_fd)
        last_fd = get_last_fd(node);
    
    child_pid = fork();

    if (child_pid == 0)
    {
        if (last_fd != -1)
            open_files(node);
        if (dup2(last_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE); 
        }
        close(last_fd);
        exit(execute_ast(node->left));
    }
    else if (child_pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    return WEXITSTATUS(status);
}