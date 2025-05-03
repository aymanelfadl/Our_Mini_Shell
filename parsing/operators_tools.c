#include <minishell.h>



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
    if (!ft_strncmp(s, ";", 1))
        return (-2);
    if (!ft_strncmp(s, "\\", 1))
        return (-2);
    return (-1);
}

int ops_size(char *s, char **all_ops)
{
    int i;

    i = 0;
    while (*s)
    {
        if (extract_ops_helper(s, all_ops) >= 0)
        {
            s += ft_strlen(all_ops[extract_ops_helper(s, all_ops)]);
            i++;
        }
        else
            s++;
    }
    return (i + 1);
}