/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_to_skip.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:01:16 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:01:16 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static int	there_is_no_next_op(char *command, int *to_skip, int *j)
{
	int	i;

	i = *j;
	if (find_next_ops(skip_ops(command)) == -1)
	{
		to_skip[i++] = 0;
		*j = i;
		return (1);
	}
	return (0);
}

static int	condition_check(char *command, int cnd_nmbr)
{
	if (cnd_nmbr == 2)
		return (there_is_something_between_2_adresses(skip_ops(command),
				skip_ops(command) + find_next_ops(skip_ops(command))));
	return (get_data_type(command) == PIPE
		&& find_next_ops(skip_ops(command)) != NON);
}

int	*get_to_skip(char *command, char **ops)
{
	int	i;
	int	*to_skip;

	i = 0;
	to_skip = ft_malloc(sizeof(int) * (double_char_size(ops) + 1));
	if (!ops || !*ops)
		return (NULL);
	while (1)
	{
		if (there_is_no_next_op(command, to_skip, &i))
			break ;
		else
			command += find_next_ops(command);
		if (condition_check(command, 1))
		{
			if (condition_check(command, 2))
				to_skip[i++] = 1;
			else
				to_skip[i++] = 0;
		}
		else
			to_skip[i++] = 0;
		command = skip_ops(command);
	}
	return (to_skip);
}
