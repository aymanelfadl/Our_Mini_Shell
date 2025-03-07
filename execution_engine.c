#include "execution.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int execute_ast(t_tree *node)
{
    // Base case: If the node is NULL, return 0
    if (node == NULL)
        return 0;

    // Check if it's a COMMAND type, then execute it
    if (node->type == COMMAND)
        return execute_command(node);
    else if (node->type == PIPE)
        return execute_pipe(node);
    
    return 0;
}

int execute_command(t_tree *node)
{
    pid_t pid;
    int status;

    if (!node->parent)
    {
        pid = fork();
        if (pid == 0)
        {
            if (execve(node->data, node->args, NULL) == -1) 
            {
                perror(node->data);
                exit(1);
            }
        }
        else if (pid > 0)
        {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                printf("Child process exited with status: %d\n", WEXITSTATUS(status));
            else
                printf("Child process did not exit normally\n");
        }
        else
        {
            perror("exec_command");
            return -1;
        }
    }
    else
    {
        if (execve(node->data, node->args, NULL) == -1)
        {
            perror(node->data);
            exit(1);
        }
    }
    return 0;
}

int execute_pipe(t_tree *node)
{
    int pip_fd[2];
    int status1, status2;
    pid_t p_child_1, p_child_2;

    if (!node->left || !node->right)
        return -1;

    if (pipe(pip_fd) == -1)
    {
        perror("pipe");
        return -1;
    }

    p_child_1 = fork();
    if (p_child_1 == 0)
    {
        close(pip_fd[0]);
        if (dup2(pip_fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        exit(execute_ast(node->left));
    }

    if (p_child_1 == -1)
    {
        perror("fork");
        return -1;
    }

    if (waitpid(p_child_1, &status1, 0) == -1)
    {
        perror("waitpid");
        return -1;
    }

    p_child_2 = fork();
    if (p_child_2 == 0)
    {
        close(pip_fd[1]);
        close(STDIN_FILENO);
        if (dup2(pip_fd[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        exit(execute_ast(node->right));
    }
    if (p_child_2 == -1)
    {
        perror("fork");
        return -1;
    }

    close(pip_fd[0]);
    close(pip_fd[1]);

    if (waitpid(p_child_2, &status2, 0) == -1)
    {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(status2))
        return WEXITSTATUS(status2);
    else
        return -1;
}

int execute_redirection(t_tree *node)
{
    // TODO: Implement redirection execution
    return 0;
}
int main()
{
    // Create a pipe command: "ls -l | grep .c"
    t_tree *root = malloc(sizeof(t_tree));
    root->type = PIPE;
    root->data = "|";

    // Left side: "ls -l"
    root->left = malloc(sizeof(t_tree));
    root->left->type = COMMAND;
    root->left->data = "/bin/ls";
    root->left->args = malloc(3 * sizeof(char*));
    root->left->args[0] = "ls";
    root->left->args[1] = "-l";
    root->left->args[2] = NULL;
    root->left->left = NULL;
    root->left->right = NULL;

    // Right side: "grep .c"
    root->right = malloc(sizeof(t_tree));
    root->right->type = COMMAND;
    root->right->data = "/bin/grep";
    root->right->args = malloc(3 * sizeof(char*));
    root->right->args[0] = "grep";
    root->right->args[1] = "rwx";
    root->right->args[2] = NULL;
    root->right->left = NULL;
    root->right->right = NULL;

    int status = execute_ast(root);
    printf("Pipeline execution completed with status: %d\n", status);

    free(root->left->args);
    free(root->left);
    free(root->right->args);
    free(root->right);
    free(root);

    return 0;
}