#include <minishell.h>

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

    i = 0;
    int *to_skip = ft_malloc(sizeof(int) * double_char_size(ops));
    if (!ops || !*ops)
        return (NULL);
    while (1)
    {
        command += find_next_ops(command);
        if (find_next_ops(skip_ops(command)) == -1)
        {
            to_skip[i++] = 0;
            break;
        }
        else if (there_is_something_between_2_adresses(skip_ops(command), skip_ops(command) + find_next_ops(skip_ops(command))) && there_is_one_word(skip_ops(skip_ops(command) + find_next_ops(skip_ops(command)))))
            to_skip[i++] = 1;
        else
            to_skip[i++] = 0;
        command = skip_ops(command);
    }
    return (to_skip);
}

t_tree *ilyas_parsing(char *phrase, t_list *envp)
{
    char **paths = extract_paths(envp);
    int *to_skip;
    t_list *lst;
    int i;
    i = 0;
    lst = NULL;
    if (*phrase == 0)
        return (NULL);
    add_history(phrase);
    phrase = parse_env(phrase, envp);
    if (*phrase == 0)
    {
        *get_exit_status() = 0;
        return (NULL);
    }
    if (!check_unexpected_token(phrase))
    {
        *get_exit_status() = 2;
    return (printf("unexpexted token \n"), NULL);
    }
    char **cc = extract_ops(phrase);
    char ***s;
    s = ft_malloc(sizeof(char **) * 2);
    s[0] = extract_files_commands_strings(phrase, cc);

    s[1] = cc;
    to_skip = get_to_skip(phrase, cc);
    t_tree *tree = make_tree(s, to_skip);
    split_tree(tree);
    add_paths_to_tree(tree, paths);
    return (tree);
}