#include "minishell.h"

void print_double_pointer(char **s)
{
    if (s == NULL)
        printf("double pointer is NULL\n");
    while (s && *s)
    {
        printf("%s", *s);
        s++;
        if (*s)
            printf(",");
    }
    printf("\n");
}
void print_tree(t_tree *tree)
{
    if (tree == NULL)
        return;
    print_tree(tree->left);
    printf("%s %d        double :", tree->data, tree->type);
    print_double_pointer(tree->args);
    if (tree->type == COMMAND)
        printf("      path : %s", tree->path);
    printf("\nnext\n");
    print_tree(tree->right);
}
void print_node(t_tree *node, int depth)
{
    if (!node)
        return;
    for (int i = 0; i < depth; i++)
        printf("  ");
    printf("[type=%d] data='%s' path='%s' ", node->type, node->data ? node->data : "(null)", node->path ? node->path : "(null)");
    printf("left=%s right=%s\n", node->left ? "yes" : "no", node->right ? "yes" : "no");

    if (node->args && node->args[0])
    {
        for (int i = 0; node->args[i]; i++)
        {
            for (int j = 0; j < depth + 1; j++)
                printf("  ");
            printf("arg[%d]='%s'\n", i, node->args[i]);
        }
    }
    if (node->redirects && node->redirects->file)
    {
        for (int i = 0; i < depth + 1; i++)
            printf("  ");
        printf("redirect: type=%d file='%s'\n", node->redirects->type, node->redirects->file);
    }
    print_node(node->left, depth + 1);
    print_node(node->right, depth + 1);
}