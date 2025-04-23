#ifndef MINISHELL_H
#define MINISHELL_H

// ==========================================================================
//                                LIBRARIES
// ==========================================================================

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
#include <linux/limits.h>

// ==========================================================================
//                              DATA STRUCTURES
// ==========================================================================

typedef enum data_type
{
    PIPE,                   // 0
    INPUT_REDIRECTION,      // 1
    APP_OUTPUT_REDIRECTION, // 2
    OUTPUT_REDIRECTION,     // 3
    APP_INPUT_REDIRECTION,  // 4
    AND,                    // 5
    OR,                     // 6
    COMMAND,                // 7
    FT_FILE,                // 8
    FT_EOF,                 // 9
} e_type;

typedef struct s_tree {
    char *path;
    char *data;
    char **args;
    char *heredoc_content;
    char **ops;
    struct s_tree *left;
    struct s_tree *right;
    struct s_tree *parent;
    enum data_type type;
    int to_skip;
} t_tree;

enum inside_what
{
    DOUBLE_QUOTES,  // "" 0
    ONE_QUOTE,      // '' 1
    INSIDE_NOTHING, //    2
};





// ==========================================================================
//                           EXECUTION ENGINE
// ==========================================================================

int execute_ast(t_tree *node);
int execute_command_or_builtin(t_tree *node);
int execute_pipe(t_tree *node, int input_fd);
int execute_redirection(t_tree *node);
int execute_logical_operators(t_tree *node);
int execute_external_command(t_tree *node);
 int execute_output_redirection(t_tree *node);

// ==========================================================================
//                              REDIRECTIONS
// ==========================================================================

// Pipe
int execute_with_pipe(t_tree *node, char *content);
pid_t fork_and_execute(t_tree *node_to_exec, int input_fd, int output_fd);
// Input
int execute_input_redirection(t_tree *node);
int execute_append_input_redirection(t_tree *node);

// Output
int execute_append_output_redirection(t_tree *node);

// Heredoc
void process_all_heredocs(t_tree *node);
int should_expand(char *delimiter);
char *handle_single_heredoc(char *delimiter, int expand);

// Redirection Utils
void create_redir_files_and_get_last(t_tree *node, int *last_fd, e_type type);
t_tree *find_most_left_cmd(t_tree *node);

// ==========================================================================
//                                BUILTINS
// ==========================================================================

int builtins_engine(t_tree *node, t_list *envp);
void ft_echo(t_tree *node);
int ft_cd(t_tree *node , t_list * envp);
void ft_export(t_tree *node, t_list **export_envp);
char* ft_pwd(void);
void ft_exit(t_tree *node);
int ft_unset(t_tree *node);
int ft_env(t_tree *node);

// Builtin Helpers
char *remove_quotes(char *string);
char *extract_key_from_string(char *content);
int export_operator(char *explited_sport);
t_list *key_is_already_exist(t_list *envp, char *key);
int is_valid_key(char *s);
void push_back(t_list **export_envp, char *splited_export);
char *get_key(char *splited_export);
t_list *initialize_env_list(char **envp);
char **list_to_char_array(t_list *list);
int is_n_option(char *n_option, int wanna_skip);

// ==========================================================================
//                                 PARSING
// ==========================================================================

t_tree *ilyas_parsing(char * command , t_list *envp);
t_tree *make_tree(char ***data , int * to_skip);
void split_tree(t_tree *tree);
void add_paths_to_tree(t_tree *tree, char **paths);
char **ft_split_files(char *files);


// Token Extraction & Handling
char **extract_ops(char *s);
char **extract_files_commands_strings(char *command, char **ops);
char *parse_env(char *s , t_list *envp);
char *handle_commandes_quoets(t_tree *tree);

// Path Handling
char **extract_paths(t_list *envp);
char *check_paths(char **paths, char **command);
int is_path(char *command);

// Validation & Checks
int commas_ops_check(char *s);
int check_unexpected_token(char *command);
enum data_type get_data_type(char *s);
int is_file(enum data_type type);
enum inside_what string_is_inside(char *s, int start);
int there_is_something_between_2_adresses(char *s1, char *s2);

// Parsing Helpers
char *skip_spaces(char *s);
char *skip_ops(char *command);
int find_next_ops(char *command);
int ops_size(char *s, char **ops);
int extract_ops_helper(char *s, char **ops);
char *replace_strin_in_string(char *s, int start_string, int end_string, char *inserted_string);

// String Utils (Consider moving to libft or a dedicated string utils file)
char *my_strnstr(const char *haystack, const char *needle, size_t len);
char *my_strchr(char *s, char *target);
int ft_strcmp(char *s1, char *s2); // Already in libft?

// ==========================================================================
//                                UTILITIES
// ==========================================================================

// Environment
char **get_envp(char **envp);
t_list *strings_to_list(char **strings);

// Process Management
int wait_for_child(pid_t child_pid);

// Memory Management
void *ft_malloc(size_t size);
int ft_free(t_list *lst);

// Debugging / Printing
void print_node(t_tree *node, int depth);
void print_strings(char **strs);

// Exit Status
int *get_exit_status(void);

// General Helpers
int double_char_size(char **s);

// ==========================================================================
//                                 SIGNALS
// ==========================================================================

void ft_set_interactive_signals();
void ctrl_d_handle();
void ft_set_default_signals();
void ft_set_wait_signals();

// ==========================================================================
//                                  MISC
// ==========================================================================

// t_tree *find_leftmost_cmd(t_tree *node); // Commented out, seems unused

#endif
