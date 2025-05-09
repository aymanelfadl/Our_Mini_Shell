/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export_helper.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:02:28 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/09 20:02:28 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

char	*remove_quotes(char *string)
{
	char	*string_removed_quotes;
	char	*next_quote_adresse;
	int		i;

	if (!ft_strchr(string, 34) && !ft_strchr(string, 39))
		return (string);
	string_removed_quotes = ft_malloc(ft_strlen(string));
	i = 0;
	while (*string)
	{
		if ((*string == 34 || *string == 39) && ft_strchr(string + 1, *string))
		{
			next_quote_adresse = ft_strchr(string + 1, *string);
			while (++string != next_quote_adresse && *string)
				string_removed_quotes[i++] = *string;
		}
		else if ((*string != 34 && *string != 39))
			string_removed_quotes[i++] = *string;
		string++;
	}
	string_removed_quotes[i] = '\0';
	return (string_removed_quotes);
}

char	*extract_key_from_string(char *content)
{
	char	*key;

	if (ft_strnstr(content, "+=", ft_strlen(content)))
		key = ft_substr(content, 0, ft_strchr(content, '=') - content);
	else if (ft_strchr(content, '='))
		key = ft_substr(content, 0, ft_strchr(content, '=') - content);
	else
		key = ft_strdup(content);
	return (key);
}

t_list	*key_is_already_exist(t_list *envp, char *key)
{
	while (envp)
	{
		if (!ft_strcmp(key, extract_key_from_string((char *)envp->content)))
			return (envp);
		envp = envp->next;
	}
	return (NULL);
}

int	is_valid_key(char *s)
{
	char	*key;
	int		equal_found;

	equal_found = 0;
	if (!ft_strchr(s, '=') && s[0] != '=')
		key = s;
	else if (ft_strchr(s, '='), s[0] != '=')
	{
		key = ft_substr(s, 0, ft_strchr(s, '=') - s);
		equal_found = 1;
	}
	else
		return (0);
	if (ft_isdigit(*key))
		return (0);
	while (*key)
	{
		if (ft_isalnum(*key) || *key == '_' || ((*key == '+') && (*(key
						+ 1) == 0) && equal_found))
			key++;
		else
			return (0);
	}
	return (1);
}
