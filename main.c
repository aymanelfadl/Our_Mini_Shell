#include "minishell.h"

t_list *garbage_collector = NULL;

void ft_free_split(char **split)
{
    if (!split)
        return;

    for (int i = 0; split[i]; i++)
        free(split[i]);

    free(split);
}
char *get_prompt(void)
{
    char cwd[PATH_MAX];
    char *prompt;

    if (!getcwd(cwd, sizeof(cwd)))
        cwd[0] = '\0';
    prompt = ft_strjoin("minishell:", cwd);
    prompt = ft_strjoin(prompt, "$> ");
    return prompt;
}



static void minishell_loop(t_list *env_list)
{
    char *input;
    char **cmds;
    int saved_stdout;
    int saved_stdin;
    char *prompt;
    while (1)
    {
        signal(SIGINT, sigint_handler);
        signal(SIGQUIT, SIG_IGN);
        prompt = get_prompt();
        input = readline(prompt);
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
            cmds = ft_split(input, "\n");
            save_std_fds(&saved_stdout, &saved_stdin);
            execute_commands(cmds, env_list);
            restore_std_fds(saved_stdout, saved_stdin);
        }
    }
}

int main(int ac, char **av, char **envp)
{
    (void)ac;
    (void)av;
    t_list *env_list;
    char **env_array;

    if (!*envp)
        env_list = initialize_env_list(get_envp(list_to_char_array(create_minimal_env())));
    else
    {
        env_list = initialize_env_list(envp);
        increment_shlvl(env_list);
        get_envp(list_to_char_array(env_list));
    }
    minishell_loop(env_list);
    return 0;
}
