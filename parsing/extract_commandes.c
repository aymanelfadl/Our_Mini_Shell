#include <minishell.h>

static int check_file_before_command_irederection(char *command)
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
static char ** allocate_commandes_files(char ** ops , char *command)
{
    int size;
    char **commandes_files;

    size = ops_size(command, ops) * 2;
    commandes_files = ft_malloc(sizeof(char *) * size);
    return (commandes_files);
}
char **extract_files_commands_strings(char *command, char **ops)
{
    int i;
    char **commandes_files;
    int first_loop;

    i = 0;
    first_loop = 1;
    commandes_files = allocate_commandes_files(ops , command);
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
