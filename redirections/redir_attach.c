#include "minishell.h"

static void attach_redir_to_cmd(t_tree *cmd_node, t_redirection *redir_list)
{
    if (!cmd_node || !redir_list)
        return;
    if (cmd_node->type == PIPE || cmd_node->type == AND || cmd_node->type == OR)
        cmd_node = cmd_node->right;
    if (!cmd_node || cmd_node->type != COMMAND)
        return;
    if (!cmd_node->redirects)
        cmd_node->redirects = redir_list;
}


void attach_all_redirections(t_tree *node)
{
    t_redirection *redir_list;
    t_tree *cmd_node;

    if (!node)
        return ;
    redir_list = NULL;
    if ((node->type == INPUT_REDIRECTION || node->type == OUTPUT_REDIRECTION ||
        node->type == APP_OUTPUT_REDIRECTION || node->type == APP_INPUT_REDIRECTION)
            && (!node->parent || !(node->parent->type == INPUT_REDIRECTION ||
            node->parent->type == OUTPUT_REDIRECTION ||
            node->parent->type == APP_OUTPUT_REDIRECTION ||
            node->parent->type == APP_INPUT_REDIRECTION)))
    {
        cmd_node = extract_redirections(node, &redir_list);
        printf("Redirection list: ");
        t_redirection *temp = redir_list;
        while (temp) {
            printf("[type: %d, file: %s] -> ", temp->type, temp->file);
            temp = temp->next;
        }
        printf("NULL\n");        
        if (cmd_node)
            attach_redir_to_cmd(cmd_node, redir_list);
        else if (redir_list)
            node->redirects = redir_list;
    }
    attach_all_redirections(node->left);
}
