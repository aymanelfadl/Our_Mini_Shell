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
void print_double_pointer(char ** s)
{
    while (s && *s)
    {
        printf ("'%s' ",*s);
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
        printf("      path : %s", tree->path);
    printf("\nnext\n");
    print_tree(tree->right);
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
    if (envp)
        saved_envp = envp;
    return saved_envp;
}

t_list *initialize_env_list(char **envp)
{
    static t_list *head;
    if (envp != NULL)
        head = strings_to_list(envp);
    return head;
}

void ft_free_split(char **split)
{
    if (!split)
        return;
        
    for (int i = 0; split[i]; i++)
        free(split[i]);
    
    free(split);
}

void print_node(t_tree *node, int depth)
{
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("[type=%d] data='%s' path='%s'\n", node->type, node->data ? node->data : "(null)", node->path ? node->path : "(null)");
    if (node->args && node->args[0]) {
        for (int i = 0; node->args[i]; i++) {
            for (int j = 0; j < depth + 1; j++) printf("  ");
            printf("arg[%d]='%s'\n", i, node->args[i]);
        }
    }
    if (node->redirects && node->redirects->file) {
        for (int i = 0; i < depth + 1; i++) printf("  ");
        printf("redirect: type=%d file='%s'\n", node->redirects->type, node->redirects->file);
    }
    print_node(node->left, depth + 1);
    print_node(node->right, depth + 1);
}

static void restore_std_fds(int saved_stdout, int saved_stdin)
{
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdout);
    close(saved_stdin);
}

static void execute_commands(char **cmds, t_list *env_list)
{
    t_tree *tree = NULL;
    int i = 0;
    while (cmds && cmds[i])
    {
        tree = ilyas_parsing(cmds[i], env_list);
        if (tree) {
            attach_all_redirections(tree);
            process_all_heredocs(tree);
            *get_exit_status() = execute_node(tree);
        }
        i++;
    }
    cleanup_heredoc_fds(tree);
}

static void minishell_loop(t_list *env_list)
{
    char *input;
    while (1)
    {
        input = readline("$> ");
        if (!input)
            ctrl_d_handle();
        char **commands = ft_split(phrase , "\n");
        while(commands && *commands)
        {
            tree = ilyas_parsing(*commands , env_list);
           // print_node(tree, 0);
            if (tree)
                print_tree(tree);
            commands++;
        }
    }
}
