#include "minishell.h"

void process_heredocs(t_tree *node) {
    if (!node)
        return;

    if (node->type == APP_INPUT_REDIRECTION) 
    {
        node->heredoc_content = handle_heredoc(node->right->data);
        if (!node->heredoc_content) 
        {
            perror("Failed to read APP_INPUT_REDIRECTION content");
            exit(EXIT_FAILURE);
        }
    }
    process_heredocs(node->left);
    process_heredocs(node->right);
}

int execute_ast(t_tree *node)
{
    if (node == NULL)
        return 0;
    process_heredocs(node);
    if (node->type == COMMAND)
        return execute_command(node);
    else if (node->type == PIPE)
        return execute_pipe(node);
    else
        return execute_redirection(node);
    
    return 0;
}

int execute_command(t_tree *node)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        if (execve(node->path, node->args, get_envp(NULL)) == -1) 
        {
            perror(node->path);
            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0)
    {
        perror("exec_command");
        return -1;
    }
    else
    {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        else
            return -1;
    }
    return 0;
}

char *handle_heredoc(char *delimiter) {
    char *line = NULL;
    char *heredoc_content = NULL;
    char *temp = NULL;
    char *line_with_newline = NULL;
    int first_line = 1;

    heredoc_content = ft_strdup("");
    if (!heredoc_content)
        return NULL;        
    while (1) 
    {
        line = readline("APP_INPUT_REDIRECTION> ");
        if (!line) 
            break;
        if (!strcmp(line, delimiter)) 
        {
            free(line);
            break;
        }
        if (!first_line) {
            line_with_newline = ft_strjoin("\n", line);
        } else {
            line_with_newline = ft_strdup(line);
            first_line = 0;
        }

        if (!line_with_newline) {
            free(line);
            free(heredoc_content);
            return NULL;
        }
        free(line);

        temp = ft_strjoin(heredoc_content, line_with_newline);
        if (!temp) 
        {
            free(line_with_newline);
            free(heredoc_content);
            return NULL;
        }    
        free(heredoc_content);
        free(line_with_newline);
        heredoc_content = temp;
    }
    return heredoc_content;
}

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
        close(pip_fd[1]);

        int input_pipe[2];
        if (pipe(input_pipe) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        
        write(input_pipe[1], heredoc_content, ft_strlen(heredoc_content));
        close(input_pipe[1]);
        
        if (dup2(input_pipe[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(input_pipe[0]);
        
        exit(execute_ast(node->left));
    }

    p_child_2 = fork();
    if (p_child_2 == 0)
    {
        close(pip_fd[1]); 
        if (dup2(pip_fd[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(pip_fd[0]);
        exit(execute_ast(node->parent->right));
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

int execute_output_redirection(t_tree *node)
{
    pid_t child_pid;
    int file_fd;
    int status;

    child_pid = fork();
    if (child_pid == 0)
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

int execute_append_output_redirection(t_tree *node)
{
    pid_t child_pid;
    int file_fd;
    int status;

    child_pid = fork();
    if (child_pid == 0)
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

int execute_input_redirection(t_tree *node)
{
    pid_t child_pid;
    int file_fd;
    int status;

    child_pid = fork();
    if (child_pid == 0)
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

int execute_redirection(t_tree *node)
{
    if (node->type == OUTPUT_REDIRECTION)
        return execute_output_redirection(node);
    else if (node->type == INPUT_REDIRECTION)
        return execute_input_redirection(node);
    else if (node->type == APP_OUTPUT_REDIRECTION)
        return execute_append_output_redirection(node);
    else if (node->type == APP_INPUT_REDIRECTION)
        return execute_append_input_redirection(node);
    else
        return -1;
}