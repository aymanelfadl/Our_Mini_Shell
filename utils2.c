#include "minishell.h"

enum data_type get_data_type(char *s)
{
    if (!strcmp(s, "|")) // use ft_strcmp later
        return (PIPE);
    if (!strcmp(s, "||")) // use ft_strcmp later
        return (OR);
    if (!strcmp(s, "&&")) // use ft_strcmp later
        return (AND);
    if (!strcmp(s, ">")) // use ft_strcmp later
        return (OUTPUT_REDIRECTION);
    if (!strcmp(s, ">>")) // use ft_strcmp later
        return (APP_OUTPUT_REDIRECTION);
    if (!strcmp(s, "<")) // use ft_strcmp later
        return (OUTPUT_REDIRECTION);
    if (!strcmp(s, "<<")) // use ft_strcmp later
        return (APP_INPUT_REDIRECTION);
    return (-1);
}

int commas_ops_check(char *s)
{
    int commas;
    if (!s)
        return (0);
    commas = 0;
    while (s && *s)
    {
        if (*s == 34)
            commas++;
        s++;
    }
    return (commas % 2 == 0);
}

char **extract_files_commands_strings(char *COMMAND, char **ops)
{
    int size = 20;
    int i;
    char **commandes_files = ft_malloc(sizeof(char **) * size);

    i = 0;
    if (!ops)
    {
        commandes_files[0] = ft_strtrim(ft_strdup(COMMAND) , " \t");
        commandes_files[1] = NULL;
        if ((commandes_files[0][0]) == 34 && ((commandes_files[0][ft_strlen(commandes_files[0]) - 1]) == 34))
            commandes_files[0] = ft_strtrim(commandes_files[0], "\x22");
        return (commandes_files);
    }
    while (*ops && size--)
    {
        if (my_strnstr(COMMAND, *ops, ft_strlen(COMMAND)))
        {
            if (!strcmp(*ops, "<"))
            {
                COMMAND = my_strnstr(COMMAND, *ops, ft_strlen(COMMAND)) + 1;
                while ((*COMMAND) && (*COMMAND == ' ') || (*COMMAND == '\t'))
                    COMMAND++;
                commandes_files[i++] = ft_substr(COMMAND, 0, my_strchr(COMMAND, " \t") - COMMAND);
                COMMAND += ft_strlen(commandes_files[i - 1]);
            }
            else
            {
                commandes_files[i] = ft_substr(COMMAND, 0, my_strnstr(COMMAND, *ops, ft_strlen(COMMAND)) - COMMAND);
                i++;
                COMMAND += (my_strnstr(COMMAND, *ops, ft_strlen(COMMAND)) - COMMAND) + ft_strlen(*ops);
            }
        }
        commandes_files[i - 1] = ft_strtrim(commandes_files[i - 1], " \t");
        if ((commandes_files[i - 1][0]) == 34 && ((commandes_files[i - 1][ft_strlen(commandes_files[i - 1]) - 1]) == 34))
            commandes_files[i - 1] = ft_strtrim(commandes_files[i - 1], "\x22");
        ops++;
    }
    commandes_files[i++] = ft_substr(COMMAND, 0, ft_strlen(COMMAND));
    commandes_files[i - 1] = ft_strtrim(commandes_files[i - 1], " \t");
    if ((commandes_files[i - 1][0]) == 34 && ((commandes_files[i - 1][ft_strlen(commandes_files[i - 1]) - 1]) == 34))
        commandes_files[i - 1] = ft_strtrim(commandes_files[i - 1], "\x22");
    commandes_files[i] = NULL;
    return (commandes_files);
}

int is_file(enum data_type type)
{
    return (type == INPUT_REDIRECTION || type == APP_OUTPUT_REDIRECTION || type == OUTPUT_REDIRECTION);
}