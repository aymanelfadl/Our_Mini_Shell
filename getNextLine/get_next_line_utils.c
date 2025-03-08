/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 17:48:28 by aelfadl           #+#    #+#             */
/*   Updated: 2025/02/18 17:48:31 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	*my_calloc(size_t num_elements, size_t element_size)
{
	void	*ptr;
	size_t	total_size;
	size_t	i;

	if (num_elements != 0 && SIZE_MAX / num_elements < element_size)
		return (NULL);
	total_size = num_elements * element_size;
	if (total_size == 1 || total_size == 0)
		return (NULL);
	ptr = malloc(total_size);
	if (!ptr)
		return (NULL);
	i = 0;
	while (total_size > i)
	{
		((unsigned char *)ptr)[i] = 0;
		i++;
	}
	return (ptr);
}

void	*my_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*d;
	unsigned char	*s;
	size_t			i;

	i = 0;
	d = (unsigned char *)dst;
	s = (unsigned char *)src;
	if (!dst && !src)
		return (NULL);
	while (i < n && d != s)
	{
		d[i] = s[i];
		i++;
	}
	return (dst);
}

size_t	my_strlen(const char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

char	*my_strjoin(char *s1, char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*str;

	len1 = 0;
	len2 = 0;
	if (s1 != NULL)
		len1 = my_strlen(s1);
	if (s2 != NULL)
		len2 = my_strlen(s2);
	str = my_calloc(len1 + len2 + 1, 1);
	if (!str)
		return (NULL);
	if (s1)
		my_memcpy(str, s1, len1);
	if (s2)
		my_memcpy(str + len1, s2, len2);
	return (str);
}

char	*my_strchr(char *str, int c)
{
	int	i;

	if (!str)
		return (NULL);
	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (&str[i]);
		i++;
	}
	return (NULL);
}
