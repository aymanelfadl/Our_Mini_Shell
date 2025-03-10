#include "execution.h"


int execute_ast(t_tree *node)
{
    if (node == NULL)
        return 0;

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
        if (execve(node->data, node->args, NULL) == -1) 
        {
            perror(node->data);
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
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        else
            printf("Child process did not exit normally\n");
    }
    return 0;
}

char *handle_heredoc(char *delimiter) {
    char *line = NULL;
    char *heredoc_content = NULL;
    char *temp = NULL;
    char *line_with_newline = NULL;

    heredoc_content = ft_strdup("");
    if (!heredoc_content)
        return NULL;        
    while (1) 
    {
        line = readline("heredoc> ");
        if (!line) {
            break;
        }        
        if (!strcmp(line, delimiter)) 
        {
            free(line);
            break;
        }
        line_with_newline = ft_strjoin(line, "\n");
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
    char *heredoc_content = NULL;

    if (!node->left)
        return -1;
    heredoc_content = handle_heredoc(node->right->data);
    if (!heredoc_content) {
        perror("handle_heredoc");
        return -1;
    }
    if (pipe(pip_fd) == -1)
    {
        perror("pipe");
        free(heredoc_content);
        exit(EXIT_FAILURE); 
    }
    p_child_1 = fork();
    if (p_child_1 == -1)
    {
        perror("fork");
        close(pip_fd[0]);
        close(pip_fd[1]);
        free(heredoc_content); 
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
    
    free(heredoc_content);

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

    if (node->left->type == APP_INPUT_REDIRECTION) {
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
    
    char *heredoc_content = handle_heredoc(node->right->data);
    if (!heredoc_content) 
    {
        perror("handle_heredoc");
        return -1;
    }    

    child_pid = fork();
    if (child_pid == 0)
    {
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("pipe");
            free(heredoc_content);
            exit(EXIT_FAILURE);
        }  

        write(pipefd[1], heredoc_content, ft_strlen(heredoc_content));
        close(pipefd[1]);
        free(heredoc_content);
            
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
        free(heredoc_content);
        return -1;
    }
    
    free(heredoc_content);
    
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

// Test program
int main(void)
{
    // Create the command node for "cat"
    t_tree *cat_cmd = malloc(sizeof(t_tree));
    if (!cat_cmd)
        exit(EXIT_FAILURE);
    cat_cmd->type = COMMAND;
    cat_cmd->data = "/bin/cat";
    cat_cmd->args = malloc(2 * sizeof(char *));
    if (!cat_cmd->args)
        exit(EXIT_FAILURE);
    cat_cmd->args[0] = "cat";
    cat_cmd->args[1] = NULL;
    cat_cmd->left = NULL;
    cat_cmd->right = NULL;
    cat_cmd->parent = NULL;

    // Create a node for the delimiter
    t_tree *delimiter_node = malloc(sizeof(t_tree));
    if (!delimiter_node)
        exit(EXIT_FAILURE);
    delimiter_node->type = COMMAND;  // Type doesn't matter much here
    delimiter_node->data = "EOF";    // This is the delimiter
    delimiter_node->args = NULL;
    delimiter_node->left = NULL;
    delimiter_node->right = NULL;
    delimiter_node->parent = NULL;

    // Create the heredoc redirection node
    t_tree *heredoc = malloc(sizeof(t_tree));
    if (!heredoc)
        exit(EXIT_FAILURE);
    heredoc->type = APP_INPUT_REDIRECTION;
    heredoc->data = NULL;
    heredoc->args = NULL;
    heredoc->left = cat_cmd;         // The command to execute
    heredoc->right = delimiter_node; // The delimiter node
    heredoc->parent = NULL;

    // Set parent pointers for the heredoc part
    cat_cmd->parent = heredoc;
    delimiter_node->parent = heredoc;

    // Create the command node for "wc -l"
    t_tree *wc_cmd = malloc(sizeof(t_tree));
    if (!wc_cmd)
        exit(EXIT_FAILURE);
    wc_cmd->type = COMMAND;
    wc_cmd->data = "/usr/bin/wc";
    wc_cmd->args = malloc(3 * sizeof(char *));
    if (!wc_cmd->args)
        exit(EXIT_FAILURE);
    wc_cmd->args[0] = "wc";
    wc_cmd->args[1] = "-l";
    wc_cmd->args[2] = NULL;
    wc_cmd->left = NULL;
    wc_cmd->right = NULL;
    wc_cmd->parent = NULL;

    // Create the pipe node
    t_tree *pipe_node = malloc(sizeof(t_tree));
    if (!pipe_node)
        exit(EXIT_FAILURE);
    pipe_node->type = PIPE;
    pipe_node->data = NULL;
    pipe_node->args = NULL;
    pipe_node->left = heredoc;    // Left side of the pipe (cat << EOF)
    pipe_node->right = wc_cmd;    // Right side of the pipe (wc -l)
    pipe_node->parent = NULL;

    // Update parent pointers for the pipe
    heredoc->parent = pipe_node;
    wc_cmd->parent = pipe_node;

    // Execute the AST. This will trigger your pipeline code.
    int status = execute_ast(pipe_node);
    printf("Pipeline execution completed with status: %d\n", status);

    // Clean up allocated memory
    free(cat_cmd->args);
    free(cat_cmd);
    free(delimiter_node);
    free(heredoc);
    free(wc_cmd->args);
    free(wc_cmd);
    free(pipe_node);

    return 0;
}