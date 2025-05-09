#include <minishell.h>

static void	allocate_for_left(int last_word, t_tree **tree)
{
	if (last_word != -1)
	{
		(*tree)->left = ft_malloc(sizeof(t_tree));
		(*tree) = (*tree)->left;
	}
}

static void	make_tree_helper(t_tree *tree, t_tree *parent, int to_skip,
		char *ops)
{
	tree->redirects = NULL;
	tree->pipe_fds[0] = -1;
	tree->pipe_fds[0] = -1;
	tree->parent = parent;
	tree->to_skip = to_skip;
	tree->data = ops;
	tree->type = get_data_type(tree->data);
	tree->right = NULL;
	tree->heredoc_content = NULL;
	tree->args = NULL;
	tree->path = NULL;
	tree->ops = NULL;
	tree->heredoc_content = NULL;
}

t_tree	*make_tree(char ***data, int *to_skip)
{
	char	**ops;
	t_tree	*tree;
	t_tree	*head;
	int		last_word;
	t_tree	*parent;

	last_word = double_char_size(data[1]) - 1;
	if (last_word == -1)
		return (create_one_node(*data[0]));
	tree = ft_malloc(sizeof(t_tree));
	head = tree;
	parent = NULL;
	ops = data[1];
	while (last_word >= 0)
	{
		make_tree_helper(tree, parent, to_skip[last_word], ops[last_word]);
		parent = tree;
		allocate_for_left(--last_word, &tree);
	}
	tree->left = NULL;
	put_to_tree(&head, data[0], double_char_size(data[0]) - 1, tree);
	return (head);
}
