/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:01:16 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:01:16 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_double_pointer(char **s)
{
	if (s == NULL)
		printf("double pointer is NULL\n");
	while (s && *s)
	{
		printf("%s", *s);
		s++;
		if (*s)
			printf(",");
	}
	printf("\n");
}

void	print_tree(t_tree *tree)
{
	if (tree == NULL)
		return ;
	print_tree(tree->left);
	printf("%s %d        double :", tree->data, tree->type);
	print_double_pointer(tree->args);
	if (tree->type == COMMAND)
		printf("      path : %s\n", tree->path);
	printf("\nnext\n");
	print_tree(tree->right);
}

int	there_is_one_word(char *command)
{
	if (*command == 34 || *command == 39)
		command = ft_strchr(command + 1, *command);
	command = skip_spaces(command);
	if (get_data_type(command) != NON || *command == 0)
		return (1);
	return (0);
}

static int	phrase_check(char *phrase)
{
	if (*phrase == 0)
	{
		*get_exit_status() = 0;
		return (0);
	}
	if (!check_unexpected_token(phrase))
	{
		*get_exit_status() = 2;
		return (ft_putstr_fd("minishell: syntax error near unexpected token\n",
				2), 0);
	}
	return (1);
}

t_tree	*ilyas_parsing(char *phrase, t_list *envp)
{
	char	**paths;
	int		*to_skip;
	char	***s;
	t_tree	*tree;

	if (*phrase == 0)
		return (NULL);
	add_history(phrase);
	phrase = parse_env(phrase, envp);
	if (phrase_check(phrase) == 0)
		return (NULL);
	s = ft_malloc(sizeof(char **) * 2);
	s[1] = extract_ops(phrase);
	s[0] = extract_files_commands_strings(phrase, s[1]);
	to_skip = get_to_skip(phrase, s[1]);
	tree = make_tree(s, to_skip);
	split_tree(tree);
	paths = extract_paths(envp);
	add_paths_to_tree(tree, paths);
	print_tree(tree);
	return (tree);
}
