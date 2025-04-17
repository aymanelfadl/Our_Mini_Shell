#include "minishell.h"

enum data_type get_data_type(char *s)
{
    if (!ft_strncmp(s, "||", 2))
        return (OR);
    if (!ft_strncmp(s, "&&", 2))
        return (AND);
    if (!ft_strncmp(s, ">>", 2))
        return (APP_OUTPUT_REDIRECTION);
    if (!ft_strncmp(s, "<<", 2))
        return (APP_INPUT_REDIRECTION);
    if (!ft_strncmp(s, "|", 1))
        return (PIPE);
    if (!ft_strncmp(s, ">", 1))
        return (OUTPUT_REDIRECTION);
    if (!ft_strncmp(s, "<", 1))
        return (INPUT_REDIRECTION);

    return (-1);
}

int commas_ops_check(char *s)
{
    int commas;
    if (!s)
        return (0);
    while (*s)
    {
        if (*s == 34 | *s == 39)
        {
            if (ft_strchr(s + 1, *s))
                s = ft_strchr(s + 1, *s) + 1;
            else
                return (0);
        }
        else
            s++;
    }
    return (1);
}

int find_next_ops(char *command)
{
    int i = 0;

    while (command[i])
    {
        if (get_data_type(&command[i]) != -1 && commas_ops_check(&command[i]))
            return (i);
        i++;
    }
    return (-1);
}
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

char *skip_ops(char *command)
{
    command = skip_spaces(command);
    if (!get_data_type(command) || get_data_type(command) == 1 || get_data_type(command) == 3)
        command++;
    else if (get_data_type(command) != -1)
        command += 2;
    command = skip_spaces(command);
    return (command);
}

int check_file_before_command_irederection(char *command)
{
    int ret = 0;
    if (get_data_type(command) != -1)
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

    file = NULL;
    my_command = NULL;
    j = *i;
    command = skip_ops(command);
    file = ft_substr(command, 0, my_strchr(command, " \t") - command);

    command += ft_strlen(file);
    command = skip_spaces(command);
    if (find_next_ops(command) != -1)
        my_command = ft_substr(command, 0, find_next_ops(command));
    else
        my_command = ft_substr(command, 0, ft_strlen(command));
    command += ft_strlen(my_command);
    commandes_files[j++] = my_command;
    commandes_files[j++] = file;
    *i = j;
    return (command);
}

char **extract_files_commands_strings(char *command, char **ops)
{
    int size = ops_size(command, ops) + 1;
    int i;
    i = 0;
    char **commandes_files = ft_malloc(sizeof(char **) * size);
    int first_loop;
    first_loop = 1;
    while (*command)
    {
        command = skip_spaces(command);
        if (first_loop && get_data_type(command) == INPUT_REDIRECTION)
            command = assign_file_and_command(command, commandes_files, &i);
        else if (check_file_before_command_irederection(command))
        {
            command = skip_ops(skip_ops(command));
            command = assign_file_and_command(command, commandes_files, &i);
        }
        else
        {
            command = skip_ops(command);
            while (command != skip_ops(command))
                command = skip_ops(command);
            if (find_next_ops(command) != -1)
                commandes_files[i++] = ft_substr(command, 0, find_next_ops(command));
            else
            {
                commandes_files[i++] = ft_substr(command, 0, ft_strlen(command));
                break;
            }
            command += ft_strlen(commandes_files[i - 1]);
        }
        first_loop = 0;
    }
    commandes_files[i] = NULL;

    return (commandes_files);
}

int is_file(enum data_type type)
{
    return (type == INPUT_REDIRECTION || type == APP_OUTPUT_REDIRECTION || type == OUTPUT_REDIRECTION);
}