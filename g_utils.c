/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   g_utils.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aelfadl <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 20:28:49 by aelfadl           #+#    #+#             */
/*   Updated: 2025/05/09 20:28:49 by aelfadl          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_node(t_tree *node, int depth)
{
	if (!node)
		return ;
	for (int i = 0; i < depth; i++)
		printf("  ");
	printf("[type=%d] data='%s' path='%s' ", node->type,
		node->data ? node->data : "(null)", node->path ? node->path : "(null)");
	printf("left=%s right=%s\n", node->left ? "yes" : "no",
		node->right ? "yes" : "no");
	if (node->args && node->args[0])
	{
		for (int i = 0; node->args[i]; i++)
		{
			for (int j = 0; j < depth + 1; j++)
				printf("  ");
			printf("arg[%d]='%s'\n", i, node->args[i]);
		}
	}
	if (node->redirects && node->redirects->file)
	{
		for (int i = 0; i < depth + 1; i++)
			printf("  ");
		printf("redirect: type=%d file='%s'\n", node->redirects->type,
			node->redirects->file);
	}
	print_node(node->left, depth + 1);
	print_node(node->right, depth + 1);
}
