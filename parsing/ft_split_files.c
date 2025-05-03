#include <minishell.h>



static char **how_many_file(char *files)
{
    int files_size;
    files_size = 0;

    while (*files)
    {
        if (*files == 1)
        {
            files = (ft_strchr(files + 1, *files)) + 1;
            files_size++;
        }
        else
        {
            while (*files && *files != ' ' && *files != '\t')
                files++;
            files_size++;
        }
        files = skip_spaces(files);
    }
    return (ft_malloc(sizeof(char *) * (files_size + 1)));
}

char **ft_split_files(char *files)
{
    char **files_splited;
    int i;

    i = 0;
    files_splited = how_many_file(files);
    while (*files)
    {
        if (*files == 1)
        {
            files_splited[i++] = ft_substr(files + 1, 0, ft_strchr(files + 1, *files) - (files + 1));
            files = (ft_strchr(files + 1, *files)) + 1;
        }
        else
        {
            if (my_strchr(files, " \t"))
                files_splited[i++] = ft_substr(files, 0, my_strchr(files, " \t") - files);
            else
                files_splited[i++] = ft_strdup(files);
            files += ft_strlen(files_splited[i - 1]);
        }
        files = skip_spaces(files);
    }
    files_splited[i] = NULL;
    return (files_splited);
    //return (remove_all_quotes(files_splited));
}
