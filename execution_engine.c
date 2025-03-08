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
    else if (node->type == OUTPUT_REDIRECTION ||
            node->type == APP_OUTPUT_REDIRECTION ||
            node->type == INPUT_REDIRECTION || 
            node->type == APP_INPUT_REDIRECTION)
        return execute_redirection(node);
    
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
                exit(EXIT_FAILURE);
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
    else if (node->parent->type == OUTPUT_REDIRECTION ||
            node->parent->type == APP_OUTPUT_REDIRECTION ||
            node->parent->type == INPUT_REDIRECTION ||
            node->parent->type == APP_INPUT_REDIRECTION)
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
        exit(EXIT_FAILURE); 
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
        exit(EXIT_FAILURE); 
    }

    if (waitpid(p_child_1, &status1, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE); 
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
        exit(EXIT_FAILURE); 
    }

    close(pip_fd[0]);
    close(pip_fd[1]);

    if (waitpid(p_child_2, &status2, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE); 
    }

    if (WIFEXITED(status2))
        return WEXITSTATUS(status2);
    else
        return -1;
}

int execute_redirection(t_tree *node)
{
    // if (!node->right || !node->left)
    //     return -1;
    
    int file_fd, child_pid, status;
    // if (node->right->type == FT_FILE)
    {
        child_pid = fork();
        if (child_pid == 0)
        {
            if (node->type == OUTPUT_REDIRECTION)
            {
                file_fd = open(node->right->data, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (file_fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE); 
                }
                if (dup2(file_fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE); 
                }
                close(file_fd);
                exit(execute_command(node->left));
            }
            else if (node->type == APP_OUTPUT_REDIRECTION)
            {
                file_fd = open(node->right->data, O_CREAT | O_WRONLY | O_APPEND, 0644);
                if (file_fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                if (dup2(file_fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(file_fd);
                exit(execute_command(node->left));       
            }
            else if (node->type == INPUT_REDIRECTION)
            {
                file_fd = open(node->right->data, O_RDONLY);
                if (file_fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);  
                }
                if (dup2(file_fd, STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE); 
                }
                close(file_fd);
                exit(execute_command(node->left));
            }
        else if (node->type == APP_INPUT_REDIRECTION)
        {
            char *delimiter = node->data;
            int pipefd[2];
            
            if (pipe(pipefd) == -1) {
                perror("pipe");
                return EXIT_FAILURE;
            }
            
            printf("heredoc> ");
            char *line = readline("");
            
            while (line && strcmp(line, delimiter) != 0)
            {
                write(pipefd[1], line, strlen(line));
                write(pipefd[1], "\n", 1);
                
                free(line);
                
                printf("heredoc> ");
                line = readline("");
            }
            
            if (line) free(line);
            close(pipefd[1]);
            
            int stdin_backup = dup(STDIN_FILENO);
            
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            
            int result = execute_command(node->left);
            
            dup2(stdin_backup, STDIN_FILENO);
            close(stdin_backup);
            
            return result;
        }
        exit(EXIT_FAILURE);
        }
        else if (child_pid == -1)
        {
            perror("fork");
            return -1;
        }
        else
        {
            if (waitpid(child_pid, &status, 0) == -1)
            {
                perror("waitpid");
                return -1;
            }
            if (WIFEXITED(status))
                return WEXITSTATUS(status);
            else
                return -1;
        }
    }
    return 0;
}


int main(void)
{
    // Create the command node for "cat"
    t_tree *cmd = malloc(sizeof(t_tree));
    if (!cmd)
        exit(EXIT_FAILURE);
    cmd->type = COMMAND;
    cmd->data = "/bin/cat";
    cmd->args = malloc(2 * sizeof(char *));
    if (!cmd->args)
        exit(EXIT_FAILURE);
    cmd->args[0] = "cat";
    cmd->args[1] = NULL;
    cmd->left = NULL;
    cmd->right = NULL;
    cmd->parent = NULL;

    t_tree *heredoc = malloc(sizeof(t_tree));
    if (!heredoc)
        exit(EXIT_FAILURE);
    heredoc->type = APP_INPUT_REDIRECTION;  
    heredoc->data = "EOF";
    heredoc->left = cmd;    // The command to execute
    heredoc->right = NULL;  // No file node needed
    heredoc->parent = NULL;

    // Set parent pointer so that the command knows it's under redirection
    cmd->parent = heredoc;

    // Execute the AST. This will trigger your heredoc code.
    int status = execute_ast(heredoc);
    printf("Heredoc execution completed with status: %d\n", status);

    // Clean up allocated memory
    free(cmd->args);
    free(cmd);
    free(heredoc);

    return 0;
}