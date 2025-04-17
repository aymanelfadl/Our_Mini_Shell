#ifndef MINISHELL_H
#define MINISHELL_H

// libs::
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../libft/libft.h"


typedef enum data_type
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
    FT_EOF,     //9
    BUILTIN,  //10
 } e_type;

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
int ft_echo(t_tree *node);
int ft_cd(t_tree *node , t_list * envp);
int ft_export(t_tree *node, t_list **export_envp);
char* ft_pwd(void);
void ft_exit(t_tree *node);
int ft_unset(t_tree *node);
int ft_env(t_tree *node);


char *remove_quotes(char *string);
char *extract_key_from_string(char *content);
int export_operator(char *explited_sport);
t_list *key_is_already_exist(t_list *envp, char *key);
int is_valid_key(char *s);
void push_back(t_list **export_envp, char *splited_export);
char *get_key(char *splited_export);

// UTILs::
char **get_envp(char **envp);
void print_node(t_tree *node);
int *get_exit_status(void);
void print_strings(char **strs);



//SIGNALS::
void ft_handle_signals();


// t_tree *find_leftmost_cmd(t_tree *node);
void create_redir_files_and_get_last(t_tree *node, int *last_fd, e_type type);

/*
    PARSING FUNS::
*/
enum inside_what
{
    DOUBLE_QUOTES,  // "" 0
    ONE_QUOTE,      // '' 1
    INSIDE_NOTHING, //    2
};

void *ft_malloc(size_t size);
int ft_free(t_list *lst);
char **extract_ops(char *s);
t_tree *make_tree(char ***data);
enum data_type get_data_type(char *s);
int commas_ops_check(char *s);
void print_double_pointer(char **s);
char **extract_files_commands_strings(char *command, char **ops);
char *my_strnstr(const char *haystack, const char *needle, size_t len);
t_tree *ilyas_parsing(char **envp);
int is_file(enum data_type type);
int is_path(char *command);
char *check_paths(char **paths, char **command);
char **extract_paths(t_list *envp);
void split_tree(t_tree *tree);
void add_paths_to_tree(t_tree *tree, char **paths);
char *my_strchr(char *s, char *target);
int ops_size(char *s, char **ops);
int extract_ops_helper(char *s, char **ops);
int ft_strcmp(char *s1, char *s2);
char *replace_strin_in_string(char *s, int start_string, int end_string, char *inserted_string);
enum inside_what string_is_inside(char *s, int start);
char *parse_env(char *s , t_list *envp);
int double_char_size(char **s);
char *skip_spaces(char *s);
char *handle_commandes_quoets(t_tree *tree);
int there_is_something_between_2_adresses(char *s1, char *s2);
char *skip_ops(char *command);
int find_next_ops(char *command);
int check_unexpected_token(char *command);
t_list *strings_to_list(char **strings);

#endif
