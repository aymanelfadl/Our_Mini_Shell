#include "minishell.h"

int     is_path(char *COMMAND)
{
        int     i;

        i = 0;
        if (COMMAND == NULL)
            return (0);
        while (COMMAND[i])
        {
                if (COMMAND[i++] == '/')
                        return (1);
        }
        return (0);
}

char     *check_paths(char **paths, char **COMMAND)
{
        int             i;
        char    *path;
        char    *FT_FILE;

        i = -1;

        if (is_path(*COMMAND))
            return ((access(*COMMAND , X_OK) == 0) ? ft_strdup(*COMMAND) : NULL);
        while (paths && paths[++i])
        {
                path = ft_strjoin(paths[i], "/");
                FT_FILE = ft_strjoin(path, *COMMAND);
                if (access(FT_FILE, X_OK) == 0)
                        return (FT_FILE);
        }
        return (NULL);
}
char    **extract_paths(char **envp)
{
        int     i;

        i = 0;
        while (envp && envp[i])
        {
                if (ft_strnstr(envp[i], "PATH=", 5))
                        return (ft_split(envp[i] + 5, ":"));
                i++;
        }
        return (NULL);
}



