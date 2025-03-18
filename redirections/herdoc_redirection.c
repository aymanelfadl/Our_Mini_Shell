#include "minishell.h"

char *handle_heredoc(char *delimiter) {
    char *heredoc_content = NULL;
    char *line = NULL;

    while (1)
    {
        line = readline("herdoc> ");
        if (!line || !ft_strcmp(line, delimiter))
            break;
        if (heredoc_content != NULL)
            heredoc_content = ft_strjoin(heredoc_content, "\n");
        heredoc_content = ft_strjoin(heredoc_content, line);        
    }
    return heredoc_content;
}

int execute_append_input_redirection(t_tree *node)
{
    pid_t child_pid;
    int status;
    
    char *heredoc_content = node->heredoc_content;
    if (!heredoc_content)
        return (perror("APP_INPUT_REDIRECTION content missing"), -1);

    child_pid = fork();
    if (child_pid == 0)
    {
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }  

        write(pipefd[1], heredoc_content, ft_strlen(heredoc_content));
        close(pipefd[1]);
            
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            close(pipefd[0]);
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]);
            
        exit(execute_ast(node->left));
    }
    else if (child_pid == -1)
    {
        perror("fork");
        return -1;
    }

    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}