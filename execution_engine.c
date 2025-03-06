#include "execution.h"

int execute_ast(t_tree *node)
{
    // TODO:
    if (node == NULL) {
        return 0;
    }

    if (node->type == COMMAND)
        return execute_command(node);  
    
    return 0;
}

int execute_command(t_tree *node)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0)
    {
        if (execve(node->data, node->args, NULL) == -1)
            return (perror(node->data), -1);
    }
    else if (pid > 0)
    {
        waitpid(pid, &status, 0);
        printf("status: %d\n", WEXITSTATUS(status));
    }
    else
        return (perror("exe_command:"), -1);

    return 0;
}
int execute_pipe(t_tree *node)
{
    // TODO:
    return 0;
}

int execute_redirection(t_tree *node)
{
    // TODO:
    return 0;
}
int main()
{
    t_tree *root = malloc(sizeof(t_tree));
    if (root == NULL) {
        perror("malloc failed");
        return 1;
    }

    root->data = "/bin/touch";
    root->args = malloc(3 * sizeof(char*));
    if (root->args == NULL) {
        perror("malloc failed for arguments");
        free(root);
        return 1;
    }

    root->args[0] = "aymnelfadl";
    root->args[1] = "hamid";
    root->args[2] = NULL;

    root->type = COMMAND;
   
    execute_ast(root);

    free(root->args);
    free(root);

    return 0;
}