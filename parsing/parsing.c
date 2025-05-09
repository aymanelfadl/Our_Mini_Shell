#include <minishell.h>

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
        printf("      path : %s\n", tree->path);
    printf("\nnext\n");
    print_tree(tree->right);
}

int there_is_one_word(char *command)
{
    if (*command == 34 || *command == 39)
        command = ft_strchr(command + 1, *command);
    command = skip_spaces(command);
    if (get_data_type(command) != NON || *command == 0)
        return (1);
    return (0);
}
int *get_to_skip(char *command, char **ops)
{
    int i;
    int *to_skip;
    i = 0;

    to_skip = ft_malloc(sizeof(int) * (double_char_size(ops) + 1));
    if (!ops || !*ops)
        return (NULL);
    while (1)
    {
        if (find_next_ops(skip_ops(command)) == -1)
        {
            to_skip[i++] = 0;
            break;
        }
        else
            command += find_next_ops(command);
        if (get_data_type(command) == PIPE && find_next_ops(skip_ops(command)) != NON)
        {
            if (there_is_something_between_2_adresses(skip_ops(command), skip_ops(command) + find_next_ops(skip_ops(command))))
                to_skip[i++] = 1;
            else
                to_skip[i++] = 0;
        }
        else
            to_skip[i++] = 0;
        command = skip_ops(command);
    }
    return (to_skip);
}

static int phrase_check(char *phrase)
{
    if (*phrase == 0)
    {
        *get_exit_status() = 0;
        return (0);
    }
    if (!check_unexpected_token(phrase))
    {
        *get_exit_status() = 2;
        return (ft_putstr_fd("minishell: syntax error near unexpected token\n", 2), 0);
    }
    return (1);
}

t_tree *ilyas_parsing(char *phrase, t_list *envp)
{
    char **paths;
    int *to_skip;
    char ***s;
    t_tree *tree;

    if (*phrase == 0)
        return (NULL);
    add_history(phrase);
    phrase = parse_env(phrase, envp);
    if (phrase_check(phrase) == 0)
        return (NULL);
    s = ft_malloc(sizeof(char **) * 2);
    s[1] = extract_ops(phrase);
    s[0] = extract_files_commands_strings(phrase, s[1]);
    to_skip = get_to_skip(phrase, s[1]);
    tree = make_tree(s, to_skip);
    split_tree(tree);
    paths = extract_paths(envp);
    add_paths_to_tree(tree, paths);
    return (tree);
}
