#include "minishell.h"

int main(int ac, char **av,char **envp)
{
    char **paths = extract_paths(envp);
    t_tree *tree =ilyas_parsing(1);
    split_tree(tree );
    add_paths_to_tree(tree, paths);
    execute_ast(tree);
    ft_free(garbage_collector);
}