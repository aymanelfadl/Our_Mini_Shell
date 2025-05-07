#include <minishell.h>

static char **how_many_file(char *files)
{
    int files_size;
    
    files_size = 0;
    while (*files)
    {
        if (*files == 1)
        {
            files = (ft_strchr(files + 1, *files)) + 1;
            files_size++;
        }
        else
        {
            while (*files && *files != ' ' && *files != '\t')
                files++;
            files_size++;
        }
        files = skip_spaces(files);
    }
    return (ft_malloc(sizeof(char *) * (files_size + 1)));
}
static char *delemiter_one_found(char *files, char **files_splited, int *j)
{
    int i;

    i = *j;
    files_splited[i++] = ft_substr(files + 1, 0, ft_strchr(files + 1, *files) - (files + 1));
    files = (ft_strchr(files + 1, *files)) + 1;
    *j = i;
    return (files);
}

char **ft_split_files(char *files)
{
    char **files_splited;
    int i;

    i = 0;
    files_splited = how_many_file(files);
    while (*files)
    {
        if (*files == 1)
            files = delemiter_one_found(files, files_splited, &i);
        else
        {
            if (my_strchr(files, " \t"))
                files_splited[i++] = ft_substr(files, 0, my_strchr(files, " \t") - files);
            else
                files_splited[i++] = ft_strdup(files);
            files += ft_strlen(files_splited[i - 1]);
        }
        files = skip_spaces(files);
    }
    files_splited[i] = NULL;
    return (files_splited);
}

static void add_files_to_args_helper(t_tree *node, t_tree **first_command_at_left, int *j)
{
    int i;
    char *quotes_to_add;

    i = *j;
    quotes_to_add = "'";
    if (my_strchr(node->args[i], "'\x22"))
    {
        if (*my_strchr(node->args[i], "'\x22") == 39)
            quotes_to_add = "\x22";
    }
    if ((*first_command_at_left) == NULL)
        (*first_command_at_left) = find_first_commamd_at_left(node->parent);
    (*first_command_at_left)->data = ft_strjoin((*first_command_at_left)->data, " ");
    (*first_command_at_left)->data = ft_strjoin((*first_command_at_left)->data, quotes_to_add);
    (*first_command_at_left)->data = ft_strjoin((*first_command_at_left)->data, node->args[i]);
    (*first_command_at_left)->data = ft_strjoin((*first_command_at_left)->data, quotes_to_add);
    i++;
    *j = i;
}
void add_files_to_args(t_tree *node)
{
    int i;
    t_tree *first_command_at_left;
    char *quotes_to_add;
    int there_is_a_comm = 0;

    i = 1;
    first_command_at_left = NULL;
    if (double_char_size(node->args) > 1)
    {
        if (!node->parent->left)
        {
            first_command_at_left = create_one_node(NULL);
            node->parent->left = first_command_at_left;
            first_command_at_left->parent = node->parent;
        }
        there_is_a_command(node->parent, &there_is_a_comm);
        while (node->args[i] && there_is_a_comm)
            add_files_to_args_helper(node, &first_command_at_left, &i);
    }
}
