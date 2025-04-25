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

// Create a new redirection node and link it to the list
// type: REDIR_INPUT, REDIR_OUTPUT, etc.
// file: filename for redirection
// list: pointer to the head of the redirection list
void add_redirection(t_redir_type type, char *file, t_redirection **list) {
    // Remove any previous redirection of the same type (for input/output)
    t_redirection **cur = list;
    while (*cur) {
        if ((*cur)->type == type) {
            t_redirection *to_free = *cur;
            *cur = (*cur)->next;
            if (to_free->file) free(to_free->file);
            free(to_free);
        } else {
            cur = &(*cur)->next;
        }
    }
    t_redirection *redir = malloc(sizeof(t_redirection));
    if (!redir) return;
    char *trimmed = file ? ft_strtrim(file, " \t\n") : NULL;
    redir->type = type;
    redir->file = trimmed ? trimmed : NULL;
    redir->fd_src = (type == REDIR_INPUT || type == REDIR_HEREDOC) ? STDIN_FILENO : STDOUT_FILENO;
    redir->fd_dst = -1;
    redir->origin_fd = -1;
    redir->next = NULL;
    // Add to the end of the list
    if (!*list) {
        *list = redir;
    } else {
        t_redirection *tail = *list;
        while (tail->next) tail = tail->next;
        tail->next = redir;
    }
}

// Apply all redirections in the list
void apply_redirections(t_redirection *rlist) {
    while (rlist) {
        int fd = -1;
        if (rlist->type == REDIR_INPUT)
            rlist->origin_fd = dup(STDIN_FILENO);
        else if (rlist->type == REDIR_OUTPUT || rlist->type == REDIR_APPEND)
            rlist->origin_fd = dup(STDOUT_FILENO);
        else
            rlist->origin_fd = -1;
        switch (rlist->type) {
            case REDIR_INPUT:
                fd = open(rlist->file, O_RDONLY);
                if (fd < 0) { perror(rlist->file); exit(1); }
                dup2(fd, STDIN_FILENO);
                break;
            case REDIR_OUTPUT:
                fd = open(rlist->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
                if (fd < 0) { perror(rlist->file); exit(1); }
                dup2(fd, STDOUT_FILENO);
                break;
            case REDIR_APPEND:
                fd = open(rlist->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
                if (fd < 0) { perror(rlist->file); exit(1); }
                dup2(fd, STDOUT_FILENO);
                break;
            case REDIR_HEREDOC:
                // Heredoc logic should be handled elsewhere
                break;
            default:
                break;
        }
        if (fd != -1) close(fd);
        rlist = rlist->next;
    }
}

// Restore all original fds in the list
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

// Free the redirection list
void free_redirections(t_redirection *rlist) {
    t_redirection *next;
    while (rlist) {
        next = rlist->next;
        if (rlist->file) free(rlist->file);
        free(rlist);
        rlist = next;
    }
}

// Recursively walk the AST and collect redirections into a linked list
// Returns a new tree with redirections removed from the tree and collected in the list
// Usage: t_redirection *redir_list = NULL; t_tree *cmd_tree = extract_redirections(tree, &redir_list);
t_tree *extract_redirections(t_tree *node, t_redirection **redir_list) {
    if (!node) return NULL;
    // If this node is a redirection node
    if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION || node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION) {
        // Add this redirection to the list
        if (node->right && node->right->data) {
            t_redir_type rtype = REDIR_NONE;
            if (node->type == INPUT_REDIRECTION) rtype = REDIR_INPUT;
            else if (node->type == OUTPUT_REDIRECTION) rtype = REDIR_OUTPUT;
            else if (node->type == APP_OUTPUT_REDIRECTION) rtype = REDIR_APPEND;
            else if (node->type == APP_INPUT_REDIRECTION) rtype = REDIR_HEREDOC;
            add_redirection(rtype, node->right->args[0], redir_list);
        }
        // Continue extracting from the left (the command subtree)
        return extract_redirections(node->left, redir_list);
    }
    // Otherwise, recursively process children
    node->left = extract_redirections(node->left, redir_list);
    node->right = extract_redirections(node->right, redir_list);
    return node;
}