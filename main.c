/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:28:49 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:28:49 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list		*g_garbage_collector = NULL;

void	ft_free_split(char **split)
{
	int	i;

	i = -1;
	if (!split)
		return ;
	while (split[++i])
		free(split[i]);
	free(split);
}

static void	handle_signales(void)
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

static char	**handle_hisotry(char *input)
{
	char	**cmds;

	add_history(input);
	cmds = ft_split(input, "\n");
	free(input);
	return (cmds);
}

static void	minishell_loop(t_list *env_list)
{
	char	*input;

	int (saved_stdin), (saved_stdout);
	while (1)
	{
		handle_signales();
		input = readline("$>");
		if (!input)
		{
			save_std_fds(&saved_stdin, &saved_stdout);
			restore_std_fds(saved_stdin, saved_stdout);
			ctrl_d_handle();
		}
		if (*skip_spaces(input) == 0)
		{
			*get_exit_status() = 0;
			free(input);
			continue ;
		}
		save_std_fds(&saved_stdin, &saved_stdout);
		execute_commands(handle_hisotry(input), env_list);
		restore_std_fds(saved_stdin, saved_stdout);
	}
}

int	main(int ac, char **av, char **envp)
{
	t_list	*env_list;

	(void)ac;
	(void)av;
	if (!*envp)
	{
		env_list = initialize_env_list(
				get_envp(list_to_char_array
					(create_minimal_env())));
	}
	else
	{
		env_list = initialize_env_list(envp);
		increment_shlvl(env_list);
		get_envp(list_to_char_array(env_list));
	}
	minishell_loop(env_list);
	return (0);
}
