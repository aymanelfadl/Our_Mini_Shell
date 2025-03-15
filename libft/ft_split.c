/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <ibennaje@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 15:31:56 by ibennaje          #+#    #+#             */
/*   Updated: 2025/03/09 03:02:58 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	check_char_is_sep(char c, char *sep)
{
	int	i;

	i = 0;
	while (sep[i])
	{
		if (sep[i] == c)
			return (1);
		i++;
	}
	return (0);
}

static size_t	count_words(char const *s, char *c)
{
	size_t	nwords;

	nwords = 0;
	while (*s)
	{
		while (check_char_is_sep(*s, c))
			s++;
		if (!check_char_is_sep(*s, c) && *s != '\0')
		{
			nwords++;
			while (*s && !check_char_is_sep(*s, c))
				s++;
		}
	}
	return (nwords);
}

static size_t	word_length(char const *word, char *c)
{
	size_t	i;

	i = 0;
	while (word[i] && !check_char_is_sep(word[i], c))
		i++;
	return (i);
}

static void	*my_free(char **forfree, size_t tozero)
{
	while (tozero > 0)
		free(forfree[--tozero]);
	free(forfree);
	return (NULL);
}

char	**ft_split(char const *s, char *c)
{
	size_t	nwords;
	size_t	wordlength;
	size_t	i;
	char	**splited;

	if (!s || !*s)
		return (NULL);
	nwords = count_words(s, c);
	splited = (char **)ft_malloc((nwords + 1) * sizeof(char *));
	if (!splited)
		return (NULL);
	i = 0;
	while (i < nwords)
	{
		while (check_char_is_sep(*s, c))
			s++;
		wordlength = word_length(s, c);
		splited[i] = ft_substr(s, 0, wordlength);
		if (!splited[i])
			return (my_free(splited, i));
		s += wordlength;
		i++;
	}
	splited[i] = NULL;
	return (splited);
}
