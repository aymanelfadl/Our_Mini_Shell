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
    // Create command node for "ls -l"
    // t_tree *node_ls = malloc(sizeof(t_tree));
    // node_ls->type = COMMAND;
    // node_ls->data = "/bin/ls";
    // node_ls->args = malloc(3 * sizeof(char *));
    // node_ls->args[0] = "ls";
    // node_ls->args[1] = "-l";
    // node_ls->args[2] = NULL;
    // node_ls->left = NULL;
    // node_ls->right = NULL;
    // node_ls->parent = NULL; // will be set later

    // // Create command node for "grep rwx"
    // t_tree *node_grep = malloc(sizeof(t_tree));
    // node_grep->type = COMMAND;
    // node_grep->data = "/bin/grep";
    // node_grep->args = malloc(3 * sizeof(char *));
    // node_grep->args[0] = "grep";
    // node_grep->args[1] = "rwx";
    // node_grep->args[2] = NULL;
    // node_grep->left = NULL;
    // node_grep->right = NULL;
    // node_grep->parent = NULL; // will be set later

    // // Create command node for "wc -l"
    // t_tree *node_wc = malloc(sizeof(t_tree));
    // node_wc->type = COMMAND;
    // node_wc->data = "/usr/bin/wc";  // Adjust path if needed
    // node_wc->args = malloc(3 * sizeof(char *));
    // node_wc->args[0] = "wc";
    // node_wc->args[1] = "-l";
    // node_wc->args[2] = NULL;
    // node_wc->left = NULL;
    // node_wc->right = NULL;
    // node_wc->parent = NULL; // will be set later

    // // Create a pipe node for "grep rwx | wc -l"
    // t_tree *pipe_grep_wc = malloc(sizeof(t_tree));
    // pipe_grep_wc->type = PIPE;
    // pipe_grep_wc->data = "|";
    // pipe_grep_wc->left = node_grep;
    // pipe_grep_wc->right = node_wc;
    // pipe_grep_wc->parent = NULL; // will be set later

    // // Set parent pointers for the lower pipe
    // node_grep->parent = pipe_grep_wc;
    // node_wc->parent = pipe_grep_wc;

    // Create the root pipe node for "ls -l | (grep rwx | wc -l)"
    t_tree *root = malloc(sizeof(t_tree));
    root->type = COMMAND;
    root->data = "/bin/ls";
    root->left = NULL;
    root->right = NULL;
    root->parent = NULL; // top-level node

    // Set parent pointers for the top-level tree
    // node_ls->parent = root;
    // pipe_grep_wc->parent = root;

    // Execute the Abstract Syntax Tree (AST)
    int status = execute_ast(root);
    printf("Nested pipeline execution completed with status: %d\n", status);

    // Free the allocated memory
    // free(node_ls->args);
    // free(node_ls);
    // free(node_grep->args);
    // free(node_grep);
    // free(node_wc->args);
    // free(node_wc);
    // free(pipe_grep_wc);
    free(root);

    return 0;
}