/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 19:12:53 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/10 19:12:55 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

// ==========================================================================
//                                LIBRARIES
// ==========================================================================

# include "../libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <linux/limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

// ==========================================================================
//                              DATA STRUCTURES
// ==========================================================================

typedef struct s_list		t_list;
// Redirection types
typedef enum e_redir_type
{
	REDIR_NONE,
	REDIR_INPUT,
	REDIR_OUTPUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}							t_redir_type;

// Redirection linked-list
typedef struct s_redirection
{
	t_redir_type			type;
	char					*file;
	int						fd_src;
	int						expand_heredoc;
	int						heredoc_fd;
	struct s_redirection	*next;
}							t_redirection;

// AST node type
typedef enum data_type
{
	PIPE,
	INPUT_REDIRECTION,
	APP_OUTPUT_REDIRECTION,
	OUTPUT_REDIRECTION,
	APP_INPUT_REDIRECTION,
	AND,
	OR,
	COMMAND,
	FT_FILE,
	FT_EOF,
	UNEXPECTED_TOKEN,
	NON
}							t_type;

// Quote context
typedef enum inside_what
{
	DOUBLE_QUOTES,
	ONE_QUOTE,
	INSIDE_NOTHING
}							t_inside_what;

// AST node
typedef struct s_tree
{
	char					*path;
	char					*data;
	char					**args;
	char					*heredoc_content;
	char					**ops;
	struct s_tree			*left;
	struct s_tree			*right;
	struct s_tree			*parent;
	t_type					type;
	int						to_skip;
	int						pipe_fds[2];
	t_redirection			*redirects;
}							t_tree;

// ==========================================================================
//                           EXECUTION ENGINE
// ==========================================================================

int							execute_node(t_tree *node);
int							execute_command(t_tree *node);
void						execute_commands(char **cmds, t_list *env_list);
int							fork_and_exec(t_tree *node);
int							execute_pipe(t_tree *node);

// SETUP ENVP
t_list						*create_minimal_env(void);
void						increment_shlvl(t_list *env_list);
t_list						*initialize_env_list(char **envp);

// UTILS
int							command_path_is_dir(char *path);
int							handle_no_path(t_tree *node);
void						save_std_fds(int *saved_stdin, int *saved_stdout);
void						restore_std_fds(int saved_stdin, int saved_stdout);
int							*get_std_fds(int stdin_fd, int stdout_fd);
void						close_saved_fds(void);

// ==========================================================================
//                              REDIRECTIONS
// ==========================================================================

// PIPE
int							execute_pipe(t_tree *node);

// Heredoc Reading
int							read_heredoc_to_pipe(const char *delimiter,
								int expand);
int							process_all_heredocs(t_tree *node);
void						cleanup_heredoc_fds(t_tree *node);
void						handle_eof_warning(const char *delimiter);
char						*expand_heredoc_line(char *line);
int							write_line_to_pipe(int pipe_fd,
								char *line_to_write);

// Redirection Application
void						add_redirection(t_redir_type type, char *file,
								t_redirection **list);
int							redir_input(t_redirection *r);
int							redir_output(t_redirection *r);
int							redir_append(t_redirection *r);
int							redir_heredoc(t_redirection *r);
int							apply_redirections(t_redirection *rlist);
t_tree						*extract_redirections(t_tree *node,
								t_redirection **redir_list);
void						attach_all_redirections(t_tree *node);

// ==========================================================================
//                                BUILTINS
// ==========================================================================

int							builtins_engine(t_tree *node, t_list *envp);
void						ft_echo(t_tree *node);
int							ft_cd(t_tree *node, t_list *envp);
int							ft_export(t_tree *node, t_list **export_envp);
int							ft_pwd(t_list *envp);
void						ft_exit(t_tree *node);
int							ft_unset(t_tree *node);
int							ft_env(t_tree *node);

// Builtin Helpers
char						*remove_quotes(char *string);
char						*extract_key_from_string(char *content);
int							export_operator(char *explited_sport);
t_list						*key_is_already_exist(t_list *envp, char *key);
int							is_valid_key(char *s);
void						push_back(t_list **export_envp,
								char *splited_export);
char						*get_key(char *splited_export);
t_list						*initialize_env_list(char **envp);
char						**list_to_char_array(t_list *list);
void						is_not_valid_key_helper(char *splited_export,
								int *exit_status);
void						export_switch_cases(char *splited_export,
								t_list **export_envp, int *exit_status);
char						*write_envp_content(char *content,
								int *equal_found);
int							ft_strtoll(const char *str, long long *result);
// ==========================================================================
//                                 PARSING
// ==========================================================================

t_tree						*ilyas_parsing(char *command, t_list *envp);
t_tree						*make_tree(char ***data, int *to_skip);
void						split_tree(t_tree *tree);
t_tree						*create_one_node(char *command);
void						add_paths_to_tree(t_tree *tree, char **paths);
char						**ft_split_files(char *files);
char						*get_original_eof(char *tree_data);
void						put_to_tree(t_tree **node, char **commands_files,
								int index, t_tree *last_node_parent);
char						*assign_file_and_command(char *command,
								char **commandes_files, int *i);
int							*get_to_skip(char *command, char **ops);

// Token Extraction & Handling
char						**extract_ops(char *s);
char						**extract_files_commands_strings(char *command,
								char **ops);
char						*parse_env(char *s, t_list *envp);
char						*handle_commandes_quoets(char *tree_data);

// Path Handling
char						**extract_paths(t_list *envp);
char						*check_paths(char **paths, char **command);
int							is_path(char *command);

// Validation & Checks
int							commas_ops_check(char *s);
int							check_unexpected_token(char *command);
enum data_type				get_data_type(char *s);
int							is_file(enum data_type type);
enum inside_what			string_is_inside(char *s, int start);
int							there_is_something_between_2_adresses(char *s1,
								char *s2);

// Parsing Helpers
char						*get_value(t_list *envp, char *key);
char						*skip_spaces(char *s);
char						*skip_ops(char *command);
int							find_next_ops(char *command);
int							ops_size(char *s, char **ops);
int							extract_ops_helper(char *s, char **ops);
char						*replace_strin_in_string(char *s, int start_string,
								int end_string, char *inserted_string);
int							isdirectory(char *path);

// String Utils (Consider moving to libft or a dedicated string utils file)
char						*my_strnstr(const char *haystack,
								const char *needle, size_t len);
char						*my_strchr(char *s, char *target);
int							ft_strcmp(char *s1, char *s2);
char						*get_value(t_list *envp, char *key);

// ==========================================================================
//                                UTILITIES
// ==========================================================================

// Environment
char						**get_envp(char **envp);
t_list						*strings_to_list(char **strings);

// Process Management
int							wait_for_child(pid_t child_pid);

// Memory Management
void						*ft_malloc(size_t size);
int							ft_free(t_list *lst);

// Debugging / Printing
void						print_node(t_tree *node, int depth);
void						print_strings(char **strs);

// Exit Status
int							*get_exit_status(void);

// General Helpers
int							double_char_size(char **s);
int							here_doc_before_dollar_sign(char *string,
								char *dollr_sign);

// ==========================================================================
//                                 SIGNALS
// ==========================================================================

void						sigint_handler(int signo);
void						ctrl_d_handle(void);
void						heredoc_sigint_handler(int sig);

#endif
