#include <minishell.h>

int there_is_something_between_2_adresses(char *s1, char *s2)
{
    while (s1 != s2)
    {
        if (*s1 != ' ' && *s1 != '\t')
            return (0);
        s1++;
    }
    return (1);
}

int check_file_before_command_irederection(char *command)
{
    int ret = 0;
    if (get_data_type(command) != NON)
    {
        if (get_data_type(skip_ops(command) + find_next_ops(command)) == INPUT_REDIRECTION)
        {
            if (there_is_something_between_2_adresses(command, command + find_next_ops(command)))
                ret = 1;
        }
    }

    return (ret);
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
    while (file_and_command[file_len])
    {
        if ((file_and_command[file_len] == ' ' || file_and_command[file_len] == '\t') && string_is_inside(file_and_command , file_len) == INSIDE_NOTHING)
            break;
        file_len++;
    }
    file = ft_substr(file_and_command, 0 , file_len);
    file_and_command+= file_len;
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
static int extract_files_commands_strings_helper(char **command, char **commandes_files, int *j)
{
    int i;

    i = *j;
    *command = skip_ops(*command);
    while (*command != skip_ops(*command))
        *command = skip_ops(*command);
    if (find_next_ops(*command) != -1)
        commandes_files[i++] = ft_substr(*command, 0, find_next_ops(*command));
    else
    {
        commandes_files[i++] = ft_substr(*command, 0, ft_strlen(*command));
        *j = i;
        return (1);
    }
    (*command) += ft_strlen(commandes_files[i - 1]);
    *j = i;
    return (0);
}
char **extract_files_commands_strings(char *command, char **ops)
{
    int size;
    int i;
    char **commandes_files;
    int first_loop;

    i = 0;
    first_loop = 1;
    size = ops_size(command, ops) + 1;
    commandes_files = ft_malloc(sizeof(char *) * size);
    while (*command)
    {
        command = skip_spaces(command);
        if (first_loop && get_data_type(command) == INPUT_REDIRECTION)
            command = assign_file_and_command(command, commandes_files, &i);
        else if (check_file_before_command_irederection(command))
            command = assign_file_and_command(skip_ops(skip_ops(command)), commandes_files, &i);
        else
        {
            if (extract_files_commands_strings_helper(&command, commandes_files, &i))
                break;
        }
        first_loop = 0;
        if (command == NULL)
            break;
    }
    commandes_files[i] = NULL;

    return (commandes_files);
}

int is_file(enum data_type type)
{
    return (type == INPUT_REDIRECTION || type == APP_OUTPUT_REDIRECTION || type == OUTPUT_REDIRECTION);
}