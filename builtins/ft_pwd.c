#include "minishell.h"

static int	get_pwd_from_env(t_list *env)
{
	char	*pwd;

	pwd = get_value(env, "PWD");
	if (pwd)
	{
		printf("%s\n", pwd);
		return (0);
	}
	else
	{
		ft_putstr_fd("cant get PWD from env\n", 2);
		return (1);
	}
	return (1);
}

int	ft_pwd(t_list *envp)
{
	char	*buffer;

	buffer = NULL;
	buffer = getcwd(NULL, 0);
	if (buffer)
	{
		printf("%s\n", buffer);
		free(buffer);
		return (0);
	}
	return (get_pwd_from_env(envp));
}
