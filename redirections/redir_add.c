/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_add.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:29:06 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:29:06 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	redir_input(t_redirection *r)
{
	int	fd;

	fd = open(r->file, O_RDONLY);
	if (fd != -1)
	{
		if (dup2(fd, STDIN_FILENO) == -1)
			return (close(fd), perror("dup2 input error"), 1);
		return (close(fd), 0);
	}
	return (perror(r->file), 1);
}

int	redir_output(t_redirection *r)
{
	int	fd;

	fd = open(r->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd != -1)
	{
		if (dup2(fd, STDOUT_FILENO) == -1)
			return (close(fd), perror("dup2 output error"), 1);
		return (close(fd), 0);
	}
	return (perror(r->file), 1);
}

int	redir_append(t_redirection *r)
{
	int	fd;

	fd = open(r->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
	if (fd != -1)
	{
		if (dup2(fd, STDOUT_FILENO) == -1)
		{
			close(fd);
			return (perror("dup2 append error"), 1);
		}
		return (close(fd), 0);
	}
	return (perror(r->file), 1);
}

int	redir_heredoc(t_redirection *r)
{
	if (dup2(r->heredoc_fd, STDIN_FILENO) == -1)
		return (perror("dup2 heredoc error"), 1);
	return (close(r->heredoc_fd), 0);
}

void	add_redirection(t_redir_type type, char *file, t_redirection **list)
{
	t_redirection	*redir;

	redir = ft_malloc(sizeof(t_redirection));
	redir->type = type;
	redir->file = file;
	if (type == REDIR_INPUT || type == REDIR_HEREDOC)
		redir->fd_src = STDIN_FILENO;
	else
		redir->fd_src = STDOUT_FILENO;
	redir->heredoc_fd = -1;
	redir->expand_heredoc = 1;
	redir->next = *list;
	*list = redir;
}
