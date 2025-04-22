#include "minishell.h"

// still need expend of the herdoc still crt +c in herdoc i hope that's all
int should_expand(char *delimiter)
{
    return (ft_strchr(delimiter, '$') != NULL);
}
char *expand_line(char *line)
{
    char *expanded_line;

    expanded_line = getenv(line+1);
    return (expanded_line);
}

char *append_to_content(char *content, char *line)
{
    char *new_content;
    
    if (!content)
        return ft_strdup(line);
    new_content = ft_strjoin(content, "\n");
    return ft_strjoin(new_content, line);
}
char *handle_single_heredoc(char *delimiter, int expand)
{
    char *content = NULL;
    char *line = NULL;

    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            free(content);
            return NULL;
        }
        if (!ft_strcmp(line, delimiter))
        {
            free(line);
            content = append_to_content(content, "\n");
            break;
        }
        if (expand && ft_strchr(line, '$'))
            line = expand_line(line);
        content = append_to_content(content, line);
        free(line);
    }
    return content;
}

void process_all_heredocs(t_tree *node)
{
    if (!node)
        return;

    if ((node->left && node->left->type == APP_INPUT_REDIRECTION) ||
        (node->right && node->right->type == APP_INPUT_REDIRECTION))
    {
        process_all_heredocs(node->left);
        process_all_heredocs(node->right);
    }
    if (node->type == APP_INPUT_REDIRECTION)
    {
        int expand = should_expand(node->right->data);
        char *heredoc_content = handle_single_heredoc(ft_strtrim(node->right->data, " "), expand);
        node->heredoc_content = heredoc_content;
    }
}


int execute_append_input_redirection(t_tree *node)
{
    pid_t child_pid;
    int status;
    
    char *heredoc_content = node->heredoc_content;
    if (!heredoc_content)
        return (printf("here-document delimited by end-of-file (wanted `%s')\n", node->right->args[0]), 0);

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
            
        exit(execute_ast(find_most_left_cmd(node->left)));
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