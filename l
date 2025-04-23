#include "minishell.h"

t_list *garbage_collector = NULL;

void print_strings(char **strs)
{
    int i = 0;
    while (strs[i])
    {
        printf("%s\n", strs[i]);
        i++;
    }
}
void print_node(t_tree *node, int depth)
{
    if (!node)
        return;

    // Indent based on depth
    for (int i = 0; i < depth; i++)
        printf("  ");

    // Print node type
    if (node->type == PIPE)
        printf("PIPE");
    else if (node->type == COMMAND)
        printf("COMMAND");
    else if (node->type == INPUT_REDIRECTION)
        printf("REDIRECT_IN");
    else if (node->type == OUTPUT_REDIRECTION)
        printf("REDIRECT_OUT");
    else if (node->type == APP_OUTPUT_REDIRECTION)
        printf("APPEND OUT");
    else if (node->type == APP_INPUT_REDIRECTION)
        printf("HERDOC");
    else if (node->type == FT_FILE) 
        printf("FILE");
    else
        printf("UNKNOWN");

    // Print command if available
    if (node->args && node->args[0])
        printf(" | cmd: %s", node->args[0]);

    // Print parent info
    if (node->parent)
    {
        printf(" | parent: ");
        if (node->parent->type == PIPE)
            printf("PIPE");
        else if (node->parent->type == COMMAND)
            printf("COMMAND");
        else if (node->parent->type == INPUT_REDIRECTION)
            printf("REDIRECT_IN");
        else if (node->parent->type == OUTPUT_REDIRECTION)
            printf("REDIRECT_OUT");
        else
            printf("UNKNOWN");
    }

    printf("\n"); // <<< Main new line here

    // Recursive calls
    print_node(node->left, depth + 1);
    print_node(node->right, depth + 1);

    // Optional: Add empty line between subtrees (helps with visual separation)
    if (depth == 0)
        printf("\n");
}

int *get_exit_status(void)
{
    static int exit_status;
    return &exit_status;
}

char **get_envp(char **envp)
{
    static char **saved_envp;
    
    if (envp != NULL) {
        saved_envp = envp;
    }
    return saved_envp;
}

t_list *initialize_env_list(char **envp)
{
    static t_list *head;
    if (envp != NULL)
        head = strings_to_list(envp);
    return head;
}

void initialize_shell(t_list **env_list, char **envp)
{
    get_envp(envp); 
    *env_list = initialize_env_list(envp); 
}
void execute_tree(t_tree *tree)
{
    if (!tree)
        return;

    // process_all_heredocs(tree);
    *get_exit_status() = execute_ast(tree);
}

int main(int ac, char **av, char **envp)
{
    (void)ac;
    (void)av;
    t_tree *tree;
    t_list *env_list;

    ft_set_interactive_signals();
    initialize_shell(&env_list, envp);
    
    while (1) {
        char *phrase;
        phrase = readline("$>");
        if (!phrase)
            ctrl_d_handle();
        char **commands = ft_split(phrase , "\n");
        while(commands && *commands)
        {
            tree = ilyas_parsing(*commands , env_list);
            print_node(tree, 0);
            if (tree)
               execute_tree(tree);
            commands++;
        }
    }
}
