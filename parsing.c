#include "minishell.h"

t_tree *ilyas_parsing(int condition)
{
    t_list *lst;
    int i;
    i = 0;
    lst = NULL;
    char *phrase;
    phrase = NULL;
    while (!commas_ops_check(phrase) || (ft_strncmp(phrase, "\n", ft_strlen(phrase) == 0)))
    {
        phrase = ft_strjoin(phrase, readline("minishell$>"));
    }

    char **cc = extract_ops(phrase);
    char ***s;
    s = ft_malloc(sizeof(char **) * 2);
    s[0] = extract_files_commands_strings(phrase, cc);
    // print_double_pointer(s[0]);
    s[1] = cc;
    t_tree *tree = make_tree(s);
    return (tree);
}
