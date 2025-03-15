#include "minishell.h"

t_list *garbage_collector = NULL;

char **get_envp(char **envp)
{
    static char **saved_envp;
    
    if (envp != NULL) {
        saved_envp = envp;
    }
    return saved_envp;
}

int main(int ac, char **av, char **envp)
{
    get_envp(envp);
    char **paths = extract_paths(envp);
    t_tree *tree = ilyas_parsing(1);
    split_tree(tree);
    add_paths_to_tree(tree, paths);
    // print_tree(tree);
    process_heredocs(tree);
    execute_ast(tree);
    ft_free(garbage_collector);
}