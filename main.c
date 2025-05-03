#include "minishell.h"

t_list *garbage_collector = NULL;

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
    printf("[type=%d] data='%s' path='%s' ", node->type, node->data ? node->data : "(null)", node->path ? node->path : "(null)");
    printf("left=%s right=%s\n", node->left ? "yes" : "no", node->right ? "yes" : "no");
    
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
            if (process_all_heredocs(tree) == -1) {
                cleanup_heredoc_fds(tree);
                *get_exit_status() = 130;
                break;
            }
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
        signal(SIGINT, sigint_handler);
        signal(SIGQUIT, SIG_IGN);
        input = readline("$> ");
        if (!input)
            ctrl_d_handle();
        if (*skip_spaces(input) == 0)
        {
            *get_exit_status() = 0;
            free(input);
            continue;
        }
        if (input && *input)
        {
            add_history(input);
            char **cmds = ft_split(input, "\n");
            int saved_stdout = dup(STDOUT_FILENO);
            int saved_stdin = dup(STDIN_FILENO);
            execute_commands(cmds, env_list);
            restore_std_fds(saved_stdout, saved_stdin);
        }
    }
}

static t_list *create_minimal_env(void)
{
    char *minimal_env[4];
    t_list *env_list;
    char *cwd;
    int i;
    
    env_list = NULL;
    cwd = getcwd(NULL, 0);
    if (!cwd)
        cwd = ft_strdup("/");
    
    minimal_env[0] = ft_strjoin("PWD=", cwd);
    minimal_env[1] = ft_strdup("SHLVL=1");
    minimal_env[2] = ft_strdup("_=/usr/bin/env");
    minimal_env[3] = NULL;

    env_list = strings_to_list(minimal_env);
    free(cwd);
    return env_list;
}

static void increment_shlvl(t_list *env_list)
{
    t_list *current;
    char *current_shlvl = NULL;
    char *new_shlvl_str = NULL;
    int shlvl_value;
    
    current = env_list;
    shlvl_value = 1;
    current_shlvl = NULL;
    new_shlvl_str = NULL;
    while (current)
    {
        if (ft_strncmp(current->content, "SHLVL=", 6) == 0)
        {
            current_shlvl = (char *)current->content + 6;
            shlvl_value = ft_atoi(current_shlvl) + 1;
            new_shlvl_str = ft_itoa(shlvl_value);
            current->content = ft_strjoin("SHLVL=", new_shlvl_str);
            return;
        }
        current = current->next;
    }
    new_shlvl_str = ft_strdup("SHLVL=1");
    ft_lstadd_back(&env_list, ft_lstnew(new_shlvl_str));
}

int main(int ac, char **av, char **envp)
{
    (void)ac; (void)av;
    t_list *env_list;
    char **env_array;
    
    if (!*envp)
        initialize_env_list(get_envp(list_to_char_array(create_minimal_env())));
    else
    {
        env_list = initialize_env_list(envp);
        increment_shlvl(env_list);
        get_envp(list_to_char_array(env_list));
    }
    minishell_loop(env_list);
    return 0;
}
