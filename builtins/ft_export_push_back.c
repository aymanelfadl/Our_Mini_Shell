#include "minishell.h"

char	*switch_case_export(char *eec, char *se, int switch_case)
{
	char	*new_content;

	if (switch_case == 1)
	{
		new_content = ft_substr(eec, 0, ft_strchr(eec, '=') + 1 - eec);
		new_content = ft_strjoin(eec, ft_strchr(se, '=') + 1);
	}
	else if (switch_case == 2)
	{
		new_content = ft_strjoin(eec, "=");
		new_content = ft_strjoin(eec, ft_strchr(se, '=') + 1);
	}
	else
	{
		new_content = ft_substr(eec, 0, ft_strlen(eec));
		new_content = ft_strjoin(eec, ft_strchr(se, '=') + 1);
	}
	return (new_content);
}

static char	*export_operator_equal_or_plus(char *export_envp_content,
		char *splited_export)
{
	int	switch_case;

	if (export_operator(splited_export) == 1)
	{
		if (ft_strchr(export_envp_content, '='))
			switch_case = 1;
		else
			switch_case = 2;
	}
	else
		switch_case = 3;
	return (switch_case_export(export_envp_content, splited_export,
			switch_case));
}

void	push_back(t_list **export_envp, char *splited_export)
{
	char	*key;
	char	*value;
	char	*export_envp_content;

	key = get_key(splited_export);
	if (key_is_already_exist(*export_envp, key))
	{
		export_envp_content = key_is_already_exist(*export_envp, key)->content;
		export_envp_content = export_operator_equal_or_plus(export_envp_content,
				splited_export);
		key_is_already_exist(*export_envp, key)->content = export_envp_content;
		return ;
	}
	if (!(*(ft_strchr(splited_export, '=') + 1)))
		value = ft_strdup("=");
	else
		value = ft_strdup(ft_strchr(splited_export, '='));
	ft_lstadd_back(export_envp, ft_lstnew(ft_strjoin(key, value)));
}
