#include "minishell.h"

int wait_for_child(pid_t child_pid)
{
    int status;
    
    if (child_pid <= 0)
        return 1;
    
    ft_set_default_signals();
    
    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        return 1;
    }
    
    ft_set_interactive_signals();
    
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    
    return 1;
}

void add_redirection(t_redir_type type, char *file, t_redirection **list) {
    t_redirection *redir = ft_malloc(sizeof(t_redirection));
    if (!redir) return;
    
    char *trimmed = file ? ft_strtrim(file, " \t\n") : NULL;
    redir->type = type;
    redir->file = trimmed ? trimmed : NULL;
    redir->fd_src = (type == REDIR_INPUT || type == REDIR_HEREDOC) ? STDIN_FILENO : STDOUT_FILENO;
    redir->fd_dst = -1;
    redir->origin_fd = -1;
    redir->next = NULL;
    
    redir->next = *list;
    *list = redir;
}

void apply_redirections(t_redirection *rlist) {
    int last_fd = -1;

    while (rlist) {
        int fd = -1;

        // Save current stdin or stdout
        if (rlist->type == REDIR_INPUT)
            rlist->origin_fd = dup(STDIN_FILENO);
        else if (rlist->type == REDIR_OUTPUT || rlist->type == REDIR_APPEND)
            rlist->origin_fd = dup(STDOUT_FILENO);

        switch (rlist->type) {
            case REDIR_INPUT:
                fd = open(rlist->file, O_RDONLY);
                if (fd != -1) {
                    dup2(fd, STDIN_FILENO); // Redirect stdin to file
                    close(fd);
                } else {
                    perror("Input file open error");
                }
                break;

            case REDIR_OUTPUT:
                fd = open(rlist->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
                if (fd != -1) {
                    if (last_fd != -1) {
                        // Chain previous redirection
                        dup2(last_fd, STDOUT_FILENO);
                        close(last_fd);
                    }
                    dup2(fd, STDOUT_FILENO); // Apply output redirection
                    close(fd);
                } else {
                    perror("Output file open error");
                }
                break;

            case REDIR_APPEND:
                fd = open(rlist->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
                if (fd != -1) {
                    if (last_fd != -1) {
                        // Chain previous redirection
                        dup2(last_fd, STDOUT_FILENO);
                        close(last_fd);
                    }
                    dup2(fd, STDOUT_FILENO); // Apply append redirection
                    close(fd);
                } else {
                    perror("Append file open error");
                }
                break;
        }

        // Keep track of the last file descriptor to chain next redirection
        last_fd = fd;

        rlist = rlist->next;
    }
}



void restore_redirections(t_redirection *rlist) {
    while (rlist) {
        if (rlist->origin_fd != -1) {
            if (rlist->type == REDIR_INPUT)
                dup2(rlist->origin_fd, STDIN_FILENO);
            else if (rlist->type == REDIR_OUTPUT || rlist->type == REDIR_APPEND)
                dup2(rlist->origin_fd, STDOUT_FILENO);
            close(rlist->origin_fd);
            rlist->origin_fd = -1;
        }
        rlist = rlist->next;
    }
}

t_tree *extract_redirections(t_tree *node, t_redirection **redir_list) {
    if (!node) return NULL;
    
    // Found a redirection node
    if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION || 
        node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION) {
        
        // Extract the file info from the right child
        if (node->right && node->right->data) {
            t_redir_type rtype = REDIR_NONE;
            if (node->type == INPUT_REDIRECTION) rtype = REDIR_INPUT;
            else if (node->type == OUTPUT_REDIRECTION) rtype = REDIR_OUTPUT;
            else if (node->type == APP_OUTPUT_REDIRECTION) rtype = REDIR_APPEND;
            else if (node->type == APP_INPUT_REDIRECTION) rtype = REDIR_HEREDOC;
            
            add_redirection(rtype, node->right->data, redir_list);
        }
        
        // The left child might be a command or another redirection
        return extract_redirections(node->left, redir_list);
    }
    
    return node;
}