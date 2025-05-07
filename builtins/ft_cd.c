#include <minishell.h>

static void update_pwds(char *old_pwd, char *pwd, t_list *envp)
{
    char *s_opwd;
    char *s_pwd;
    s_opwd = ft_strjoin("OLDPWD=", old_pwd);
    s_pwd = ft_strjoin("PWD=", pwd);
    free(old_pwd);
    free(pwd);
    push_back(&envp, s_opwd);
    push_back(&envp, s_pwd);
}
static int cd_one_arg(char *pwd, char *old_pwd, t_list *envp)
{
    pwd = get_value(envp, "HOME");
    if (!pwd)
    {
        free(old_pwd);
        ft_putstr_fd("minishell : cd : HOME not set\n", 2);
        return (1);
    }
    else
    {
        if (chdir(pwd) == 0)
        {
            update_pwds(old_pwd, pwd, envp);
            return (0);
        }
        else
            perror("chdir");
    }
    return (1);
}
static int cd_error(char *arg)
{
    ft_putstr_fd("minishell: cd: ", 2);
    perror(arg);
    return(1);
}

int ft_cd(t_tree *node, t_list *envp)
{
    char *old_pwd;
    char *pwd;

    if (double_char_size(node->args) > 2)
        return (ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO), 1);
    old_pwd = getcwd(NULL, 0);
    pwd = NULL;
    if (double_char_size(node->args) == 1)
        return (cd_one_arg(pwd, old_pwd, envp));
    else if (chdir(node->args[1]) == 0)
    {
        pwd = getcwd(NULL, 0);
        if (!pwd)
        {
            ft_putstr_fd("minishell : cd : cannot access parent dir \n", 2);
            return (1);
        }
        update_pwds(old_pwd, pwd, envp);
        return (0);
    }
    return (cd_error(node->args[1]));
}
