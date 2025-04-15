#include "minishell.h"

// void INThandler(int sig)
// {
//     char c;
//     signal(sig, SIG_IGN);
//     signal(SIGINT, INThandler);
//     ilyas_parsing(0);
// }

t_tree *ilyas_parsing(char **envp)
{
    char **paths = extract_paths(envp);
    t_list *lst;
    int i;
    i = 0;
    lst = NULL;
    char *phrase;
    phrase = readline("$>");
    // add_history(phrase);
    phrase = parse_env(phrase);
    if (!check_unexpected_token(phrase))
        return ( printf("unexpexted token \n"),NULL);
    char **cc = extract_ops(phrase);
    char ***s;
    s = ft_malloc(sizeof(char **) * 2);
    s[0] = extract_files_commands_strings(phrase, cc);
    s[1] = cc;

    t_tree *tree = make_tree(s);
    split_tree(tree);
    add_paths_to_tree(tree, paths);
    // printf("***************** tree ********************** \n\n");
    // print_tree(tree);
    return (tree);
}

// int main(int ac, char **av, char **envp)
// {

//     int i = 0;
//     t_tree *tree;
//     while ((tree = ilyas_parsing(envp)))
//     {
//         i++;
//     }
//     return (0);
// }

// // built ins tests

// int main(int ac, char **av)
// {
//     t_tree node;
//     while (1)
//     {
//         node.data = readline("$>");
//         if (!commas_ops_check(node.data))
//             return (printf("syntax error near unexpected token \n"), 1);
//         node.s = ft_split(node.data, " \t");
//         ft_echo(&node);
//     }
// }

// int main(int ac , char **av)
// {
//     t_tree node;
//     node.data =  readline("$>");
//     node.s = ft_split(node.data , " \t");
//     ft_cd(&node);

// }

// int main(int ac , char **av)
// {
//     ft_pwd();
// }

//EXPORT
// t_list *strings_to_list(char **strings)
// {
//     t_list *head;
//     head = NULL;
//     while (strings && *strings)
//     {
//         ft_lstadd_back(&head, ft_lstnew(*strings));
//         strings++;
//     }
//     return (head);
// }
// int main(int ac, char **av, char **envp)
// {
//     t_tree node;
//     t_list *env;
//     env = strings_to_list(envp);
//     while (1)
//     {
//         node.data = readline("$>");
//         node.s = ft_split(node.data, " \t");
//         ft_export(&node, &env);
//     }
// }