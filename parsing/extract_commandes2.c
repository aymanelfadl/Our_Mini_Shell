#include <minishell.h>

static int get_file_len(char *file_and_command, int *len)
{
    int file_len;

    file_len = *len;
    while (file_and_command[file_len])
    {
        if ((file_and_command[file_len] == ' ' || file_and_command[file_len] == '\t') && string_is_inside(file_and_command, file_len) == INSIDE_NOTHING)
            break;
        file_len++;
    }
    *len = file_len;
    return (file_len);
}
static char *first_check(char *command, char *file_and_command)
{
    command = skip_spaces(skip_ops(command));
    if (find_next_ops(command) != -1)
    {
        file_and_command = ft_substr(command, 0, find_next_ops(command));
        command += find_next_ops(command);
    }
    else
    {
        file_and_command = command;
        command = NULL;
    }
    return (command);
}

char *assign_file_and_command(char *command, char **commandes_files, int *i)
{
    int j;
    char *file;
    char *my_command;
    char *file_and_command;
    int file_len;

    file_len = 0;
    file = NULL;
    my_command = NULL;
    j = *i;
    command = first_check(command, file_and_command);
    file_len = get_file_len(file_and_command, &file_len);
    file = ft_substr(file_and_command, 0, file_len);
    file_and_command += file_len;
    file_and_command = skip_spaces(file_and_command);
    if (*file_and_command)
    {
        my_command = ft_substr(file_and_command, 0, ft_strlen(file_and_command));
        commandes_files[j++] = my_command;
    }
    commandes_files[j++] = file;
    *i = j;
    return (command);
}
