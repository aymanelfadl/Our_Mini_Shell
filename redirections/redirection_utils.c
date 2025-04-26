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

void add_redirection(t_redir_type type, char *file, t_redirection **list)
{
    t_redirection *redir;
    char *trimmed;

    redir = ft_malloc(sizeof(t_redirection));
    if (!redir) 
        return;
    redir->type = type;
    redir->file = file;
    if (type == REDIR_INPUT || type == REDIR_HEREDOC)
        redir->fd_src = STDIN_FILENO;
    else
        redir->fd_src = STDOUT_FILENO;
    redir->fd_dst = -1;
    redir->origin_fd = -1;
    redir->next = NULL;
    redir->next = *list;
    *list = redir;
}

int apply_redirections(t_redirection *rlist)
{
    int fd;

    while (rlist)
    {
        fd = -1;
        if (rlist->type == REDIR_INPUT)
            rlist->origin_fd = dup(STDIN_FILENO);
        else if (rlist->type == REDIR_OUTPUT || rlist->type == REDIR_APPEND)
            rlist->origin_fd = dup(STDOUT_FILENO);
        if (rlist->type == REDIR_INPUT) {
            fd = open(rlist->file, O_RDONLY);
            if (fd != -1)
            {
                dup2(fd, STDIN_FILENO);
                close(fd);
            } 
            else
                return (perror("Output file open error"), 1);
        }
        else if (rlist->type == REDIR_OUTPUT)
        {
            fd = open(rlist->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
            if (fd != -1)
            {
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            else
                return (perror("Output file open error"), 1);
        }
        else if (rlist->type == REDIR_APPEND)
        {
            fd = open(rlist->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
            if (fd != -1)
            {
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            else
                return (perror("Append file open error"), 1);
        }
        rlist = rlist->next;
    }
    return 0;
} 

void restore_redirections(t_redirection *rlist) {
    while (rlist)
    {
        if (rlist->origin_fd != -1)
        {
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

t_tree *extract_redirections(t_tree *node, t_redirection **redir_list) 
{
    t_redir_type rtype;

    if (!node)
        return NULL;
    if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION || 
        node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION) 
    {
        if (node->right && node->right->args)
        {
            rtype = REDIR_NONE;
            if (node->type == INPUT_REDIRECTION) 
                rtype = REDIR_INPUT;
            else if (node->type == OUTPUT_REDIRECTION) 
                rtype = REDIR_OUTPUT;
            else if (node->type == APP_OUTPUT_REDIRECTION) 
                rtype = REDIR_APPEND;
            else if (node->type == APP_INPUT_REDIRECTION) 
                rtype = REDIR_HEREDOC;
            printf("Adding redirection: type=%d, file=%s\n", rtype, node->right->args[0]);
            add_redirection(rtype, ft_strtrim(node->right->args[0], " \t\n") , redir_list);
        }
        return extract_redirections(node->left, redir_list);
    }
    return node;
}

void attach_all_redirections(t_tree *node) 
{
    t_redirection *redir_list;
    t_tree *cmd_node;

    if (!node) 
        return;
    redir_list = NULL;
    if (node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION || 
        node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION) 
    {    
        cmd_node = extract_redirections(node, &redir_list);
		if (cmd_node && cmd_node->type == PIPE)
        {
			cmd_node->right->redirects = redir_list;
    	    attach_all_redirections(node->left);
        }
        else if (cmd_node && cmd_node->type == COMMAND)
            cmd_node->redirects = redir_list;
    }
    else
	    attach_all_redirections(node->left);
}
