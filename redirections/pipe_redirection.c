#include "minishell.h"

int execute_simple_pipe(t_tree *node)
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
    if (p_child_1 == -1)
    {
        perror("fork");
        close(pip_fd[0]);
        close(pip_fd[1]);
        exit(EXIT_FAILURE); 
    }
    
    if (p_child_1 == 0)
    {
        close(pip_fd[0]); 
        if (dup2(pip_fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(pip_fd[1]);
        exit(execute_ast(node->left));
    }

    p_child_2 = fork();
    if (p_child_2 == -1)
    {
        perror("fork");
        close(pip_fd[0]);
        close(pip_fd[1]);
        exit(EXIT_FAILURE); 
    }
    
    if (p_child_2 == 0)
    {
        close(pip_fd[1]); 
        if (dup2(pip_fd[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(pip_fd[0]);
        exit(execute_ast(node->right));
    }
    
    close(pip_fd[0]);
    close(pip_fd[1]);
    
    if (waitpid(p_child_1, &status1, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE); 
    }
    
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

int execute_heredoc_pipe(t_tree *node)
{
    int pip_fd[2];
    int status1, status2;
    pid_t p_child_1, p_child_2;
    
    if (!node->left)
        return -1;
        
    char *heredoc_content = node->heredoc_content;
    if (!heredoc_content) {
        perror("APP_INPUT_REDIRECTION content not found");
        return -1;
    }
    
    if (pipe(pip_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    write(pip_fd[1], heredoc_content, ft_strlen(heredoc_content));

    p_child_2 = fork();
    if (p_child_2 == 0) {
        close(pip_fd[1]);
        dup2(pip_fd[0], STDIN_FILENO);
        close(pip_fd[0]);
        exit(execute_ast(node->parent->right));
    }

    close(pip_fd[0]);
    close(pip_fd[1]);

    if (waitpid(p_child_2, &status2, 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status2))
        return WEXITSTATUS(status2);
    else
        return -1;
}


int execute_pipe(t_tree *node)
{
    if (!node->left || !node->right)
        return -1;

    if (node->left->type == APP_INPUT_REDIRECTION || node->right->type == APP_INPUT_REDIRECTION) {
        return execute_heredoc_pipe(node->left);
    } else {
        return execute_simple_pipe(node);
    }
}