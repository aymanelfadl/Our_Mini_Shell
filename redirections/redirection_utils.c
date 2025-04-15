#include "minishell.h"

void create_redir_files_and_get_last(t_tree *node, int *last_fd, e_type type)
{
    int fd;

    if (node->left && ( node->left->type == OUTPUT_REDIRECTION || node->left->type == APP_OUTPUT_REDIRECTION))
        create_redir_files_and_get_last(node->left, last_fd, type);
    if (type == OUTPUT_REDIRECTION)
        fd = open(node->right->data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else if (type == APP_OUTPUT_REDIRECTION)
        fd = open(node->right->data, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror(node->right->data);
        exit(EXIT_FAILURE);
    }
    if (*last_fd != -1)
        close(*last_fd);
    *last_fd = fd;
}
