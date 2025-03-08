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
    else if (node->type == OUTPUT_REDIRECTION)
        return execute_input_redirection(node);
    
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
    else if (node->parent->type == PIPE)
    {
        if (execve(node->data, node->args, NULL) == -1)
        {
            perror(node->data);
            exit(1);
        }
    }
    else if (node->parent->type == OUTPUT_REDIRECTION)
    {
        printf("%s", node->data);
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

int execute_input_redirection(t_tree *node)
{
    // TODO: Implement redirection execution
    if (!node->right)
        return -1;
    
    int outfile_fd, child_pid, status;
    if (node->right->type == FT_FILE)
    {
        child_pid = fork();
        if (child_pid == 0)
        {
            outfile_fd = open(node->right->data, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (outfile_fd == -1)
            {
                perror("open");
                return -1;
            }
            if (dup2(outfile_fd, STDOUT_FILENO) == -1 )
            {
                perror("dup2");
                return -1;
            }
            close(outfile_fd);
            exit(execute_command(node->left));
        }
        else if (child_pid == -1)
        {
            perror("fork");
            return -1;
        }
        if (waitpid(child_pid, &status, 0) == -1)
        {
            perror("waitpid");
            return -1;
        }
    }
    return 0;
}

int main(void)
{
    // Create the command node for "ls -l"
    t_tree *cmd = malloc(sizeof(t_tree));
    cmd->type = COMMAND;
    cmd->data = "/bin/ls";  // Adjust path if necessary
    cmd->args = malloc(3 * sizeof(char *));
    cmd->args[0] = "ls";
    cmd->args[1] = "-l";
    cmd->args[2] = NULL;
    cmd->left = NULL;
    cmd->right = NULL;
    cmd->parent = NULL;

    // Create the file node for "text.output"
    t_tree *file = malloc(sizeof(t_tree));
    file->type = FT_FILE;      // Assume FILE is defined in execution.h for file nodes
    file->data = "out.text";  // The file where output will be redirected
    file->args = NULL;
    file->left = NULL;
    file->right = NULL;
    file->parent = NULL;

    // Create the redirection node representing "ls -l > text.output"
    t_tree *redir = malloc(sizeof(t_tree));
    redir->type = OUTPUT_REDIRECTION;  // Using this type for redirection (even though it's output redirection)
    redir->data = ">";
    redir->left = cmd;    // The command whose output is to be redirected
    redir->right = file;  // The file to which the output will be written
    redir->parent = NULL;

    // Set parent pointers so that the command knows it's part of a redirection
    cmd->parent = redir;
    file->parent = redir;

    // Execute the AST; this should run "ls -l" with its output redirected to "text.output"
    int status = execute_ast(redir);
    printf("Output redirection execution completed with status: %d\n", status);

    // Clean up allocated memory
    free(cmd->args);
    free(cmd);
    free(file);
    free(redir);

    return 0;
}