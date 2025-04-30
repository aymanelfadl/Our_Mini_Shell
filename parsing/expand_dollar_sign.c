#include <minishell.h>

static char *handle_ted(t_list *envp, char *command)
{
    char *command_head = command;
    char *ted_adresse;

    ted_adresse = ft_strchr(command, '~');
    while (ted_adresse)
    {
        if (string_is_inside(command, ted_adresse - command) == INSIDE_NOTHING)
        {
            if ((ted_adresse == command_head) && (my_strchr(ted_adresse, " \t") == (ted_adresse + 1)))
            {
                command = replace_strin_in_string(command, ted_adresse - command, ted_adresse - command + 1, get_value(envp, "HOME"));
                ted_adresse = ft_strchr(command, '~');
            }
            else if ((*(ted_adresse - 1) == ' ' || *(ted_adresse - 1) == '\t') && (my_strchr(ted_adresse, " \t") == (ted_adresse + 1)))
            {
                command = replace_strin_in_string(command, ted_adresse - command, ted_adresse - command + 1, get_value(envp, "HOME"));
                ted_adresse = ft_strchr(command, '~');
            }
            else
                ted_adresse = ft_strchr(ted_adresse + 1, '~');
        }
        else
            ted_adresse = ft_strchr(ted_adresse + 1, '~');
    }
    return (command);
}

static int handled_exit_status(char **dollr_sign, char **s)
{
    if (*(*dollr_sign + 1) == '?')
    {
        *s = replace_strin_in_string(*s, (int)(*dollr_sign - *s), (*dollr_sign - *s + 2), ft_itoa(*get_exit_status()));
        *dollr_sign = ft_strchr(*s, '$');
    }
    else if (((ft_isalnum(*(*dollr_sign + 1))) || (*(*dollr_sign + 1) == '_')))
    {
        return (1);
    }
    else
        *dollr_sign = ft_strchr(*dollr_sign + 1, '$');

    return (0);
}

static int is_it_to_expand(char *s, char *dollr_sign)
{
    return ((string_is_inside(s, (int)(dollr_sign - s)) == DOUBLE_QUOTES) || string_is_inside(s, (int)(dollr_sign - s)) == INSIDE_NOTHING);
}

static void if_is_it_to_expand_true(char **dollr_sign, char **s, t_list *envp)
{
    int i;
    int start_end_shift;
    char *to_replace;
    int start;
    int end;

    start_end_shift = 0;
    i = 1;
    while (ft_isalnum(dollr_sign[0][i]) || dollr_sign[0][i] == '_')
        i++;
    to_replace = get_value(envp, ft_substr(*dollr_sign + 1, 0, i - 1));
    if ((string_is_inside(*s, (int)(*dollr_sign - *s)) == DOUBLE_QUOTES))
        start_end_shift = 1;
    start = (int)(*dollr_sign - *s) - start_end_shift;
    end = (int)(*dollr_sign - *s + i) + start_end_shift;
    *s = replace_strin_in_string(*s, start, end, to_replace);
    *dollr_sign = ft_strchr(*s, '$');
}
char *parse_env(char *s, t_list *envp)
{
    char *dollr_sign;
    s = handle_ted(envp, s);
    dollr_sign = ft_strchr(s, '$');
    while (dollr_sign)
    {
        if (handled_exit_status(&dollr_sign, &s))
        {
            if (is_it_to_expand(s, dollr_sign))
                if_is_it_to_expand_true(&dollr_sign, &s, envp);
            else
                dollr_sign = ft_strchr(dollr_sign + 1, '$');
        }
    }
    return (s);
}
