#ifndef EXECUTION_H
#define EXECUTION_H

// libs::
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "libft/libft.h"
#include "getNextLine/get_next_line.h"
#include <readline/readline.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>


// structs:
enum data_type
{
    COMMAND,
    PIPE,
    FT_FILE,
    OUTPUT_REDIRECTION,
    INPUT_REDIRECTION,
    APP_OUTPUT_REDIRECTION,
    APP_INPUT_REDIRECTION
};
typedef struct s_tree {
    char *data;
    char **args;         
    char *heredoc_content;
    struct s_tree *left;
    struct s_tree *right;
    struct s_tree *parent;
    enum data_type type;
} t_tree;

/*
    execution engine funcs::
        execution engine in the context of a shell or interpreter 
        is a component responsible for executing commands by managing processes.
*/
int execute_ast(t_tree *node);
int execute_command(t_tree *node);
int execute_pipe(t_tree *node);
int execute_simple_pipe(t_tree *node);
int execute_heredoc_pipe(t_tree *node);
int execute_redirection(t_tree *node);
int execute_output_redirection(t_tree *node);
int execute_append_output_redirection(t_tree *node);
int execute_input_redirection(t_tree *node);
int execute_append_input_redirection(t_tree *node);
void process_heredocs(t_tree *node);
char *handle_heredoc(char *delimiter);





#endif