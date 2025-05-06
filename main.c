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

int *get_std_fds(int stdin_fd, int stdout_fd)
{
    static int fds[2] = {-1, -1};
    
    if (stdin_fd >= 0 && stdout_fd >= 0) {
        fds[0] = stdin_fd;
        fds[1] = stdout_fd;
    }
    
    return fds;
}

void save_std_fds(int *saved_stdin, int *saved_stdout)
{
    *saved_stdin = dup(STDIN_FILENO);
    *saved_stdout = dup(STDOUT_FILENO);
    get_std_fds(*saved_stdin, *saved_stdout);
}

void	close_saved_fds(void)
{
	int	*fds;

	fds = get_std_fds(-1, -1);
	if (fds[0] >= 0)
		close(fds[0]);
	if (fds[1] >= 0)
		close(fds[1]);
}

void restore_std_fds(int saved_stdin, int saved_stdout)
{
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdin);
    close(saved_stdout);
}

static void minishell_loop(t_list *env_list)
{
    char *input;
    char **cmds;
    int saved_stdin;
    int saved_stdout;

    while (1)
    {    
        signal(SIGINT, sigint_handler);
        signal(SIGQUIT, SIG_IGN);
        input = readline("$>"); 
        if (!input)
        {
            restore_std_fds(saved_stdin, saved_stdout);
            ctrl_d_handle();
        }
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
            save_std_fds(&saved_stdin, &saved_stdout);
            execute_commands(cmds, env_list);
            restore_std_fds(saved_stdin, saved_stdout);
        }
        free(input);
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
