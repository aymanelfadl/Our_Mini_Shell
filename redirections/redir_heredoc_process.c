/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_heredoc_process.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:29:06 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:29:06 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	mark_heredocs_as_failed(t_tree *node)
{
	t_redirection	*temp;

	if (!node || !node->redirects)
		return ;
	temp = node->redirects;
	while (temp)
	{
		if (temp->type == REDIR_HEREDOC)
			temp->heredoc_fd = -1;
		temp = temp->next;
	}
}

static void	process_heredoc_redirects(t_tree *node)
{
	t_redirection	*redir;
	int				heredoc_fd;

	if (!node || !node->redirects)
		return ;
	redir = node->redirects;
	while (redir)
	{
		if (redir->type == REDIR_HEREDOC)
		{
			if (redir->heredoc_fd != -1)
			{
				close(redir->heredoc_fd);
				redir->heredoc_fd = -1;
			}
			heredoc_fd = read_heredoc_to_pipe(redir->file,
					redir->expand_heredoc);
			if (heredoc_fd == -1)
				return (mark_heredocs_as_failed(node));
			redir->heredoc_fd = heredoc_fd;
		}
		redir = redir->next;
	}
}

static int	check_heredoc_failures(t_tree *node)
{
	t_redirection	*redir;

	if (!node || !node->redirects)
		return (0);
	redir = node->redirects;
	while (redir)
	{
		if (redir->type == REDIR_HEREDOC && redir->heredoc_fd == -1)
			return (-1);
		redir = redir->next;
	}
	return (0);
}

int	process_all_heredocs(t_tree *node)
{
	if (!node)
		return (0);
	if (node->left && process_all_heredocs(node->left) == -1)
		return (-1);
	if (node->redirects)
	{
		process_heredoc_redirects(node);
		if (check_heredoc_failures(node) == -1)
			return (-1);
	}
	return (0);
}
