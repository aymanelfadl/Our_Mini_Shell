#include <minishell.h>

static void update_pwds(char *old_pwd, char *pwd, t_list *envp)
{
    push_back(&envp, old_pwd);
    push_back(&envp, pwd);
}
int ft_cd(t_tree *node, t_list *envp)
{
    char *old_pwd = malloc(PATH_MAX);
    char *pwd = malloc(PATH_MAX);
    if (getcwd(old_pwd, PATH_MAX))
        old_pwd = ft_strjoin("OLDPWD=", old_pwd);
    else
    {
        perror("getcwd");
        return (free(old_pwd), free(pwd), 1);
    }
    if (double_char_size(node->args) > 2)
        return (printf("bash: cd: too many arguments\n"), 1);
    if (!node->args[1])
        return (1);
    if (chdir(node->args[1]) == -1)
        perror(node->args[1]);
    if (getcwd(pwd, PATH_MAX))
        pwd = ft_strjoin("PWD=", pwd);
    else
    {
        perror("getcwd");
        return (free(old_pwd), free(pwd), 1);
    }
    return (update_pwds(old_pwd, pwd, envp),free(old_pwd), free(pwd) , 0);
}
