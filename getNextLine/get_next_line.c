/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 17:48:18 by aelfadl           #+#    #+#             */
/*   Updated: 2025/02/18 17:48:21 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	my_free(char **buffer)
{
	if (*buffer)
	{
		free(*buffer);
		*buffer = NULL;
	}
}

char	*get_after(char *buffer)
{
	char	*line_location;
	char	*after;
	size_t	len;

	line_location = my_strchr(buffer, '\n');
	if (!line_location || line_location[1] == '\0')
		return (NULL);
	line_location++;
	len = my_strlen(line_location);
	after = my_calloc(len + 1, 1);
	if (!after)
		return (my_free(&buffer), NULL);
	my_memcpy(after, line_location, len);
	return (after);
}

char	*get_before(char *buffer)
{
	char	*before;
	int		i;

	i = 0;
	if (!buffer)
		return (NULL);
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;
	before = my_calloc(i + 1, 1);
	if (!before)
		return (NULL);
	my_memcpy(before, buffer, i);
	return (before);
}

char	*get_full_line(int fd, char *rbuf)
{
	int		return_bytes;
	char	*buffer;
	char	*tmp;

	buffer = my_calloc((size_t)(BUFFER_SIZE + 1), 1);
	if (!buffer)
		return (my_free(&rbuf), NULL);
	while (1)
	{
		return_bytes = read(fd, buffer, BUFFER_SIZE);
		if (return_bytes < 0)
			return (my_free(&buffer), my_free(&rbuf), NULL);
		if (return_bytes == 0)
			break ;
		buffer[return_bytes] = '\0';
		tmp = my_strjoin(rbuf, buffer);
		my_free(&rbuf);
		rbuf = tmp;
		if (!rbuf)
			return (my_free(&buffer), NULL);
		if (my_strchr(rbuf, '\n'))
			break ;
	}
	my_free(&buffer);
	return (rbuf);
}

char	*get_next_line(int fd)
{
	static char	*buffer;
	char		*line;
	char		*remaining;

	if (fd < 0 || BUFFER_SIZE <= 0 || BUFFER_SIZE >= INT_MAX)
		return (NULL);
	buffer = get_full_line(fd, buffer);
	if (!buffer)
		return (NULL);
	if (buffer && !*buffer)
	{
		my_free(&buffer);
		return (NULL);
	}
	if (my_strchr(buffer, '\n'))
	{
		line = get_before(buffer);
		remaining = get_after(buffer);
		my_free(&buffer);
		buffer = remaining;
		return (line);
	}
	line = buffer;
	buffer = NULL;
	return (line);
}
