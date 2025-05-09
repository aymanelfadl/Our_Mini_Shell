#include <minishell.h>

int	ft_free(t_list *lst)
{
	t_list	*helper;

	while (lst)
	{
		free(lst->content);
		helper = lst;
		lst = lst->next;
		free(helper);
	}
	return (1);
}

void	*ft_malloc(size_t size)
{
	t_list	*new;

	new = malloc(sizeof(t_list));
	new->content = malloc(size);
	new->next = NULL;
	ft_lstadd_back(&garbage_collector, new);
	return (new->content);
}

int	double_char_size(char **s)
{
	int	i;

	i = 0;
	while (s && *s)
	{
		s++;
		i++;
	}
	return (i);
}

int	there_is_something_between_2_adresses(char *s1, char *s2)
{
	while (s1 != s2)
	{
		if (*s1 != ' ' && *s1 != '\t')
			return (0);
		s1++;
	}
	return (1);
}

int	is_file(enum data_type type)
{
	return (type == INPUT_REDIRECTION || type == APP_OUTPUT_REDIRECTION
		|| type == OUTPUT_REDIRECTION);
}
