#include "minishell.h"

int ft_free(t_list *lst)
{
    t_list *helper;
    while (lst)
    {
        free(lst->content);
        helper = lst;
        lst = lst->next;
        free(helper);
    }
    return (1);
}

void *ft_malloc(size_t size)
{
    t_list *new;
    new = malloc(sizeof(t_list));
    new->content = malloc(size);
    new->next = NULL;
    ft_lstadd_back(&garbage_collector, new);
    return (new->content);
}

int extract_ops_helper(char *s, char **ops)
{
    int i;

    i = 0;
    while (*ops)
    {
        if (my_strnstr(s, *ops, ft_strlen(*ops)))
            return (i);
        i++;
        ops++;
    }
    return (-1);
}
char **extract_ops(char *s)
{
    char *all_ops[8] = {">>", "<<", ">", "<", "||", "|", "&&", NULL};
    char **ops;
    int i;
    int op_found;
    int size;

    size = ops_size(s , all_ops);
    op_found = 0;
    i = 0;
    ops = ft_malloc((sizeof(char **) * size));
    while (*s)
    {
        if (extract_ops_helper(s, all_ops) >= 0)
        {
            ops[i++] = ft_strdup(all_ops[extract_ops_helper(s, all_ops)]);
            s += ft_strlen(all_ops[extract_ops_helper(s, all_ops)]);
            op_found = 1;
        }
        else
            s++;
    }
    ops[i] = NULL;
    if (!op_found)
        ops = NULL;
    return (ops);
}

void put_to_tree(t_tree **node, char **commands_files, int index, int one_node , t_tree * last_node_parent)
{

    if (one_node)
    {
        (*node)->right = NULL;
        (*node)->data = commands_files[index];
        (*node)->type = COMMAND;
        return;
    }
    if ((*node) != NULL)
    {
        put_to_tree(&((*node)->left), commands_files, index - 1, one_node, last_node_parent);
        (*node)->right = ft_malloc(sizeof(t_tree));
        (*node)->right->parent = (*node);
        (*node)->right->data = commands_files[index];
        (*node)->right->type = (is_file((*node)->type) ? FT_FILE : COMMAND);
        if ((*node)->type == APP_INPUT_REDIRECTION)
            (*node)->right->type = FT_EOF;
        (*node)->right->left = NULL;
        (*node)->right->right = NULL;
    }
    if ((*node) == NULL)
    {
        (*node) = ft_malloc(sizeof(t_tree));
        (*node)->data = commands_files[index];
        (*node)->parent = last_node_parent;
        (*node)->type =((*node)->parent->type == INPUT_REDIRECTION) ? FT_FILE : COMMAND;
        (*node)->left = NULL;
        (*node)->right = NULL;
        return;
    }
}

static int double_char_size(char **s)
{
    int i;
    i = 0;
    while (s && *s)
    {
        s++;
        i++;
    }
    return (i);
}
void print_double_pointer(char **s)
{
    if (s == NULL)
        printf("double pointer is NULL\n");
    while (s && *s)
    {
        printf("%s,", *s);
        s++;
    }
    printf("\n");
}

t_tree *make_tree(char ***data)
{
    char **commands_files;
    char **ops;
    t_tree *tree;
    t_tree *head;
    int last_word;
    t_tree *parent;

    last_word = double_char_size(data[1]) - 1;
    tree = ft_malloc(sizeof(t_tree));
    head = tree;
    parent = NULL;
    commands_files = data[0];
    ops = data[1];
    while (last_word >= 0)
    {
        tree->parent = parent;
        parent = tree;
        tree->data = ops[last_word--];
        tree->type = get_data_type(tree->data);
        if (last_word != -1)
        {
            tree->left = ft_malloc(sizeof(t_tree));
            tree = tree->left;
        }
    }
    tree->left = NULL;
    put_to_tree(&head, commands_files, double_char_size(commands_files) - 1, (double_char_size(commands_files) - 1 == 0) && (ops == NULL), tree);
    return (head);
}
