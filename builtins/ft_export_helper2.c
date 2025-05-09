#include <minishell.h>

void	is_not_valid_key_helper(char *splited_export, int *exit_status)
{
	ft_putstr_fd("export :", 2);
	ft_putstr_fd(splited_export, 2);
	ft_putstr_fd(" : not a valid identifier\n", 2);
	*exit_status = 1;
}

void	export_switch_cases(char *splited_export, t_list **export_envp,
		int *exit_status)
{
	if (!ft_strchr(splited_export, '=') && is_valid_key(splited_export)
		&& !key_is_already_exist(*export_envp, splited_export))
		ft_lstadd_back(export_envp, ft_lstnew(splited_export));
	else if ((ft_strnstr(splited_export, "=", ft_strlen(splited_export))
			|| ft_strnstr(splited_export, "+=", ft_strlen(splited_export)))
		&& is_valid_key(splited_export))
		push_back(export_envp, splited_export);
	else if (!is_valid_key(splited_export))
		is_not_valid_key_helper(splited_export, exit_status);
}

int	export_operator(char *explited_sport)
{
	if (ft_strchr(explited_sport, '=') && *(ft_strchr(explited_sport, '=')
			- 1) == '+')
		return (2);
	return (1);
}

char	*write_envp_content(char *content, int *equal_found)
{
	while (*content)
	{
		ft_putchar_fd(*content, 1);
		if (*content == '=')
		{
			ft_putchar_fd('\x22', 1);
			*equal_found = 1;
		}
		content++;
	}
	return (content);
}
