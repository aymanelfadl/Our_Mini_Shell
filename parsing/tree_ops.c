#include <minishell.h>

char **ft_split_files(char *files)
{
    int files_size;
    char **files_splited;
    char *head = files;
    files_size = 0;
    int i = 0;

    while (*files)
    {
        if (*files == 34 || *files == 39)
        {
            files = (ft_strchr(files + 1, *files));
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
    files_splited = ft_malloc(sizeof(char *) * (files_size + 1));
    files = head;
    while (*files)
    {
        if (*files == 34 || *files == 39)
        {
            files_splited[i++] = ft_substr(files + 1, 0, ft_strchr(files + 1, *files) - (files + 1));
            files = (ft_strchr(files + 1, *files));
            files_size++;
        }
        else
        {
            if (my_strchr(files, " \t"))
                files_splited[i++] = ft_substr(files, 0, my_strchr(files, " \t") - files);
            else
                files_splited[i++] = ft_strdup(files);

            while (*files && (*files != ' ' && *files != '\t'))
                files++;
            files_size++;
        }
        files = skip_spaces(files);
    }
    files_splited[i] = NULL;
    return (files_splited);
}

void split_tree(t_tree *tree)
{
    if (tree == NULL)
        return;
    if (tree->type == COMMAND)
        tree->data = handle_commandes_quoets(tree);
    if (!(tree->type == FT_FILE))
        tree->args = ft_split(tree->data, " \t");
    if (tree->type == FT_FILE)
        tree->args = ft_split_files(tree->data);
    split_tree(tree->right);
    split_tree(tree->left);
}
void print_tree(t_tree *tree)
{
    if (tree == NULL)
        return;
    print_tree(tree->left);
    printf("%s %d        double :", tree->data, tree->type);
    if (tree->type == COMMAND)
        printf("      path : %s", tree->path);
    printf("\nnext\n");
    print_tree(tree->right);
}

void add_paths_to_tree(t_tree *tree, char **paths)
{
    if (tree == NULL)
        return;
    if (tree->type == COMMAND)
    {
        tree->path = check_paths(paths, tree->args);
    }
    add_paths_to_tree(tree->right, paths);
    add_paths_to_tree(tree->left, paths);
}