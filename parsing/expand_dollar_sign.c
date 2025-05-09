/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_dollar_sign.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:01:16 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:01:16 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static int	is_it_to_expand(char *s, char *dollr_sign)
{
	int	there_is_end_of_before;

	there_is_end_of_before = 0;
	if (here_doc_before_dollar_sign(s, dollr_sign) == 0)
		return (0);
	if (string_is_inside(s, dollr_sign - s) == DOUBLE_QUOTES)
	{
		if (*(dollr_sign + 1) == 34 && (*(dollr_sign - 1)) == 34)
			return (0);
	}
	return ((string_is_inside(s, (int)(dollr_sign - s)) == DOUBLE_QUOTES)
		|| string_is_inside(s, (int)(dollr_sign - s)) == INSIDE_NOTHING);
}

static int	handled_exit_status(char **dollr_sign, char **s)
{
	if (*(*dollr_sign + 1) == '?' && is_it_to_expand(*s, *dollr_sign))
	{
		*s = replace_strin_in_string(*s, (int)(*dollr_sign - *s), (*dollr_sign
					- *s + 2), ft_itoa(*get_exit_status()));
		*dollr_sign = ft_strchr(*s, '$');
	}
	else if (((ft_isalnum(*(*dollr_sign + 1))) || (*(*dollr_sign + 1) == '_')
			|| (*(*dollr_sign + 1) == 34) || (*(*dollr_sign + 1) == 39)))
	{
		return (1);
	}
	else
		*dollr_sign = ft_strchr(*dollr_sign + 1, '$');
	return (0);
}

static void	if_is_it_to_expand_true(char **dollr_sign, char **s, t_list *envp)
{
	int		i;
	int		start_end_shift;
	char	*to_replace;
	int		start;
	int		end;

	start_end_shift = 0;
	i = 1;
	if (ft_isdigit(dollr_sign[0][i]))
		i = 2;
	else
	{
		while (ft_isalnum(dollr_sign[0][i]) || dollr_sign[0][i] == '_')
			i++;
	}
	if (i == 1)
		to_replace = NULL;
	else
		to_replace = get_value(envp, ft_substr(*dollr_sign + 1, 0, i - 1));
	start = (int)(*dollr_sign - *s);
	end = (int)(*dollr_sign - *s + i);
	*s = replace_strin_in_string(*s, start, end, to_replace);
	*dollr_sign = ft_strchr(*s, '$');
}

char	*parse_env(char *s, t_list *envp)
{
	char	*dollr_sign;

	dollr_sign = ft_strchr(s, '$');
	while (dollr_sign)
	{
		if (handled_exit_status(&dollr_sign, &s))
		{
			if (is_it_to_expand(s, dollr_sign))
				if_is_it_to_expand_true(&dollr_sign, &s, envp);
			else
				dollr_sign = ft_strchr(dollr_sign + 1, '$');
		}
	}
	return (s);
}
