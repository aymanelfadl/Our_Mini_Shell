#ifndef MINISHELL_H
#define MINISHELL_H

// libs::
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../libft/libft.h"


enum data_type
{
    PIPE,         //0
    INPUT_REDIRECTION,    //1
    APP_OUTPUT_REDIRECTION,          //2
    OUTPUT_REDIRECTION,    //3
    APP_INPUT_REDIRECTION,         //4
    AND,         //5
    OR,              //6 
    COMMAND,         //7
    FT_FILE,            //8
    FT_EOF       //9
};

typedef struct s_tree {
    char *path;
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
        execution engine in the context of a shell OR interpreter 
        is a component responsible for executing commands by managing processes.
*/

int execute_ast(t_tree *node);
int execute_command(t_tree *node);
int execute_pipe(t_tree *node);
int execute_redirection(t_tree *node);
int execute_logical_operators(t_tree *node);

/*
    Redirection functions::
        Handle input/output redirection in the shell.
*/

// pipe::
int execute_simple_pipe(t_tree *node);
int execute_heredoc_pipe(t_tree *node);

// input::
int execute_input_redirection(t_tree *node);
int execute_append_input_redirection(t_tree *node);

// output::
int execute_output_redirection(t_tree *node);
char *handle_heredoc(char *delimiter);
void process_heredocs(t_tree *node);
int execute_append_output_redirection(t_tree *node);

// Builtins::
int builtins_engine(t_tree *node);
int ft_unset(t_tree *node);

// UTILs::
char **get_envp(char **envp);
int ft_strcmp(const char *str1, const char *str2);
void print_node(t_tree *node);

/*
    PARSING FUNS::
*/
void *ft_malloc(size_t size);
int ft_free(t_list *lst);
char **extract_ops(char *s);
void print_tree(t_tree *tree);
t_tree *make_tree(char ***data);
enum data_type get_data_type(char *s);
int commas_ops_check(char *s);
void print_double_pointer(char **s);
char **extract_files_commands_strings(char *command, char **ops);
char *my_strnstr(const char *haystack, const char *needle, size_t len);
t_tree *ilyas_parsing(int condition);
int is_file(enum data_type type);
int     is_path(char *command);
char     *check_paths(char **paths, char **command);
char    **extract_paths(char **envp);
void split_tree(t_tree *tree);
void add_paths_to_tree(t_tree * tree , char **paths);
char *my_strchr(char *s, char *target);
int ops_size(char *s, char **ops);
int extract_ops_helper(char *s, char **ops);

#endif
