/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 17:48:07 by aelfadl           #+#    #+#             */
/*   Updated: 2025/02/18 17:48:10 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <limits.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

/* get_next_line_utils.c */
size_t	my_strlen(const char *str);
char	*my_strjoin(char *s1, char *s2);
char	*my_strchr(char *str, int c);
void	*my_calloc(size_t count, size_t size);
void	*my_memcpy(void *dst, const void *src, size_t n);
/* main function */
void	my_free(char **buffer);
char	*get_after(char *buffer);
char	*get_before(char *buffer);
char	*get_full_line(int fd, char *rbuf);
char	*get_next_line(int fd);
#endif
