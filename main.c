#include "execution.h"

// Helper function to create a basic command node
t_tree *create_cmd_node(char *path, char **args)
{
    t_tree *node = malloc(sizeof(t_tree));
    if (!node) exit(EXIT_FAILURE);
    node->type = COMMAND;
    node->data = path;
    node->args = args;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}

// Helper function to create a simple file node
t_tree *create_file_node(char *filename)
{
    t_tree *node = malloc(sizeof(t_tree));
    if (!node) exit(EXIT_FAILURE);
    node->type = COMMAND; // Using COMMAND type for simplicity
    node->data = filename;
    node->args = NULL;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}

// Helper function to create pipe nodes
t_tree *create_pipe_node(t_tree *left, t_tree *right)
{
    t_tree *node = malloc(sizeof(t_tree));
    if (!node) exit(EXIT_FAILURE);
    node->type = PIPE;
    node->data = NULL;
    node->args = NULL;
    node->left = left;
    node->right = right;
    node->parent = NULL;
    left->parent = node;
    right->parent = node;
    return node;
}

// Helper function to create redirection nodes
t_tree *create_redir_node(int type, t_tree *left, t_tree *right)
{
    t_tree *node = malloc(sizeof(t_tree));
    if (!node) exit(EXIT_FAILURE);
    node->type = type;
    node->data = NULL;
    node->args = NULL;
    node->left = left;
    node->right = right;
    node->parent = NULL;
    left->parent = node;
    right->parent = node;
    return node;
}

// Command 1: cat /etc/passwd | grep root | wc -l > root_count.txt
t_tree *create_command1(void)
{
    // cat command
    char **cat_args = malloc(3 * sizeof(char *));
    if (!cat_args) exit(EXIT_FAILURE);
    cat_args[0] = "cat";
    cat_args[1] = "/etc/passwd";
    cat_args[2] = NULL;
    t_tree *cat_cmd = create_cmd_node("/bin/cat", cat_args);
    
    // grep command
    char **grep_args = malloc(3 * sizeof(char *));
    if (!grep_args) exit(EXIT_FAILURE);
    grep_args[0] = "grep";
    grep_args[1] = "root";
    grep_args[2] = NULL;
    t_tree *grep_cmd = create_cmd_node("/usr/bin/grep", grep_args);
    
    // wc command
    char **wc_args = malloc(3 * sizeof(char *));
    if (!wc_args) exit(EXIT_FAILURE);
    wc_args[0] = "wc";
    wc_args[1] = "-l";
    wc_args[2] = NULL;
    t_tree *wc_cmd = create_cmd_node("/usr/bin/wc", wc_args);
    
    // output file
    t_tree *output_file = create_file_node("root_count.txt");
    
    // Create pipe: cat | grep
    t_tree *pipe1 = create_pipe_node(cat_cmd, grep_cmd);
    
    // Create pipe: (cat | grep) | wc
    t_tree *pipe2 = create_pipe_node(pipe1, wc_cmd);
    
    // Create redirection: ((cat | grep) | wc) > output_file
    t_tree *redir = create_redir_node(OUTPUT_REDIRECTION, pipe2, output_file);
    
    printf("\n=== EXECUTING: cat /etc/passwd | grep root | wc -l > root_count.txt ===\n");
    return redir;
}

// Command 2: cat << EOF | sort | uniq -c > sorted_unique.txt
t_tree *create_command2(void)
{
    // cat command
    char **cat_args = malloc(2 * sizeof(char *));
    if (!cat_args) exit(EXIT_FAILURE);
    cat_args[0] = "cat";
    cat_args[1] = NULL;
    t_tree *cat_cmd = create_cmd_node("/bin/cat", cat_args);
    
    // delimiter
    t_tree *delimiter = create_file_node("EOF");
    
    // heredoc
    t_tree *heredoc = create_redir_node(APP_INPUT_REDIRECTION, cat_cmd, delimiter);
    
    // sort command
    char **sort_args = malloc(2 * sizeof(char *));
    if (!sort_args) exit(EXIT_FAILURE);
    sort_args[0] = "sort";
    sort_args[1] = NULL;
    t_tree *sort_cmd = create_cmd_node("/usr/bin/sort", sort_args);
    
    // uniq command
    char **uniq_args = malloc(3 * sizeof(char *));
    if (!uniq_args) exit(EXIT_FAILURE);
    uniq_args[0] = "uniq";
    uniq_args[1] = "-c";
    uniq_args[2] = NULL;
    t_tree *uniq_cmd = create_cmd_node("/usr/bin/uniq", uniq_args);
    
    // output file
    t_tree *output_file = create_file_node("sorted_unique.txt");
    
    // Create pipe: heredoc | sort
    t_tree *pipe1 = create_pipe_node(heredoc, sort_cmd);
    
    // Create pipe: (heredoc | sort) | uniq
    t_tree *pipe2 = create_pipe_node(pipe1, uniq_cmd);
    
    // Create redirection: ((heredoc | sort) | uniq) > output_file
    t_tree *redir = create_redir_node(OUTPUT_REDIRECTION, pipe2, output_file);
    
    printf("\n=== EXECUTING: cat << EOF | sort | uniq -c > sorted_unique.txt ===\n");
    printf("Enter lines of text (type EOF on a line by itself to finish):\n");
    return redir;
}

// Command 3: ls -la | grep ^d | wc -l > dir_count.txt
t_tree *create_command3(void)
{
    // ls command
    char **ls_args = malloc(3 * sizeof(char *));
    if (!ls_args) exit(EXIT_FAILURE);
    ls_args[0] = "ls";
    ls_args[1] = "-la";
    ls_args[2] = NULL;
    t_tree *ls_cmd = create_cmd_node("/bin/ls", ls_args);
    
    // grep command
    char **grep_args = malloc(3 * sizeof(char *));
    if (!grep_args) exit(EXIT_FAILURE);
    grep_args[0] = "grep";
    grep_args[1] = "^d";
    grep_args[2] = NULL;
    t_tree *grep_cmd = create_cmd_node("/usr/bin/grep", grep_args);
    
    // wc command
    char **wc_args = malloc(3 * sizeof(char *));
    if (!wc_args) exit(EXIT_FAILURE);
    wc_args[0] = "wc";
    wc_args[1] = "-l";
    wc_args[2] = NULL;
    t_tree *wc_cmd = create_cmd_node("/usr/bin/wc", wc_args);
    
    // output file
    t_tree *output_file = create_file_node("dir_count.txt");
    
    // Create pipe: ls | grep
    t_tree *pipe1 = create_pipe_node(ls_cmd, grep_cmd);
    
    // Create pipe: (ls | grep) | wc
    t_tree *pipe2 = create_pipe_node(pipe1, wc_cmd);
    
    // Create redirection: ((ls | grep) | wc) > output_file
    t_tree *redir = create_redir_node(OUTPUT_REDIRECTION, pipe2, output_file);
    
    printf("\n=== EXECUTING: ls -la | grep ^d | wc -l > dir_count.txt ===\n");
    return redir;
}

// Command 4: cat << MARKER | grep test | sort -r | head -n 2 > top_results.txt
t_tree *create_command4(void)
{
    // cat command
    char **cat_args = malloc(2 * sizeof(char *));
    if (!cat_args) exit(EXIT_FAILURE);
    cat_args[0] = "cat";
    cat_args[1] = NULL;
    t_tree *cat_cmd = create_cmd_node("/bin/cat", cat_args);
    
    // delimiter
    t_tree *delimiter = create_file_node("MARKER");
    
    // heredoc
    t_tree *heredoc = create_redir_node(APP_INPUT_REDIRECTION, cat_cmd, delimiter);
    
    // grep command
    char **grep_args = malloc(3 * sizeof(char *));
    if (!grep_args) exit(EXIT_FAILURE);
    grep_args[0] = "grep";
    grep_args[1] = "test";
    grep_args[2] = NULL;
    t_tree *grep_cmd = create_cmd_node("/usr/bin/grep", grep_args);
    
    // sort command
    char **sort_args = malloc(3 * sizeof(char *));
    if (!sort_args) exit(EXIT_FAILURE);
    sort_args[0] = "sort";
    sort_args[1] = "-r";
    sort_args[2] = NULL;
    t_tree *sort_cmd = create_cmd_node("/usr/bin/sort", sort_args);
    
    // head command
    char **head_args = malloc(4 * sizeof(char *));
    if (!head_args) exit(EXIT_FAILURE);
    head_args[0] = "head";
    head_args[1] = "-n";
    head_args[2] = "2";
    head_args[3] = NULL;
    t_tree *head_cmd = create_cmd_node("/usr/bin/head", head_args);
    
    // output file
    t_tree *output_file = create_file_node("top_results.txt");
    
    // Create pipe: heredoc | grep
    t_tree *pipe1 = create_pipe_node(heredoc, grep_cmd);
    
    // Create pipe: (heredoc | grep) | sort
    t_tree *pipe2 = create_pipe_node(pipe1, sort_cmd);
    
    // Create pipe: ((heredoc | grep) | sort) | head
    t_tree *pipe3 = create_pipe_node(pipe2, head_cmd);
    
    // Create redirection: (((heredoc | grep) | sort) | head) > output_file
    t_tree *redir = create_redir_node(OUTPUT_REDIRECTION, pipe3, output_file);
    
    printf("\n=== EXECUTING: cat << MARKER | grep test | sort -r | head -n 2 > top_results.txt ===\n");
    printf("Enter lines of text (type MARKER on a line by itself to finish):\n");
    return redir;
}

// Command 5: find . -type f | grep ".c$" | sort | xargs wc -l | tail -n 1 > total_lines.txt
t_tree *create_command5(void)
{
    // find command
    char **find_args = malloc(4 * sizeof(char *));
    if (!find_args) exit(EXIT_FAILURE);
    find_args[0] = "find";
    find_args[1] = ".";
    find_args[2] = "-type";
    find_args[3] = "f";
    find_args[4] = NULL;
    t_tree *find_cmd = create_cmd_node("/usr/bin/find", find_args);
    
    // grep command
    char **grep_args = malloc(3 * sizeof(char *));
    if (!grep_args) exit(EXIT_FAILURE);
    grep_args[0] = "grep";
    grep_args[1] = "\\.c$";
    grep_args[2] = NULL;
    t_tree *grep_cmd = create_cmd_node("/usr/bin/grep", grep_args);
    
    // sort command
    char **sort_args = malloc(2 * sizeof(char *));
    if (!sort_args) exit(EXIT_FAILURE);
    sort_args[0] = "sort";
    sort_args[1] = NULL;
    t_tree *sort_cmd = create_cmd_node("/usr/bin/sort", sort_args);
    
    // xargs wc command
    char **xargs_args = malloc(4 * sizeof(char *));
    if (!xargs_args) exit(EXIT_FAILURE);
    xargs_args[0] = "xargs";
    xargs_args[1] = "wc";
    xargs_args[2] = "-l";
    xargs_args[3] = NULL;
    t_tree *xargs_cmd = create_cmd_node("/usr/bin/xargs", xargs_args);
    
    // tail command
    char **tail_args = malloc(4 * sizeof(char *));
    if (!tail_args) exit(EXIT_FAILURE);
    tail_args[0] = "tail";
    tail_args[1] = "-n";
    tail_args[2] = "1";
    tail_args[3] = NULL;
    t_tree *tail_cmd = create_cmd_node("/usr/bin/tail", tail_args);
    
    // output file
    t_tree *output_file = create_file_node("total_lines.txt");
    
    // Create the pipes and redirection
    t_tree *pipe1 = create_pipe_node(find_cmd, grep_cmd);
    t_tree *pipe2 = create_pipe_node(pipe1, sort_cmd);
    t_tree *pipe3 = create_pipe_node(pipe2, xargs_cmd);
    t_tree *pipe4 = create_pipe_node(pipe3, tail_cmd);
    t_tree *redir = create_redir_node(OUTPUT_REDIRECTION, pipe4, output_file);
    
    printf("\n=== EXECUTING: find . -type f | grep \".c$\" | sort | xargs wc -l | tail -n 1 > total_lines.txt ===\n");
    return redir;
}

// Command 6: cat << EOT | grep -v "^#" | sort > config.txt
t_tree *create_command6(void)
{
    // cat command
    char **cat_args = malloc(2 * sizeof(char *));
    if (!cat_args) exit(EXIT_FAILURE);
    cat_args[0] = "cat";
    cat_args[1] = NULL;
    t_tree *cat_cmd = create_cmd_node("/bin/cat", cat_args);
    
    // delimiter
    t_tree *delimiter = create_file_node("EOT");
    
    // heredoc
    t_tree *heredoc = create_redir_node(APP_INPUT_REDIRECTION, cat_cmd, delimiter);
    
    // grep command
    char **grep_args = malloc(4 * sizeof(char *));
    if (!grep_args) exit(EXIT_FAILURE);
    grep_args[0] = "grep";
    grep_args[1] = "-v";
    grep_args[2] = "^#";
    grep_args[3] = NULL;
    t_tree *grep_cmd = create_cmd_node("/usr/bin/grep", grep_args);
    
    // sort command
    char **sort_args = malloc(2 * sizeof(char *));
    if (!sort_args) exit(EXIT_FAILURE);
    sort_args[0] = "sort";
    sort_args[1] = NULL;
    t_tree *sort_cmd = create_cmd_node("/usr/bin/sort", sort_args);
    
    // output file
    t_tree *output_file = create_file_node("config.txt");
    
    // Create pipe: heredoc | grep
    t_tree *pipe1 = create_pipe_node(heredoc, grep_cmd);
    
    // Create pipe: (heredoc | grep) | sort
    t_tree *pipe2 = create_pipe_node(pipe1, sort_cmd);
    
    // Create redirection: ((heredoc | grep) | sort) > output_file
    t_tree *redir = create_redir_node(OUTPUT_REDIRECTION, pipe2, output_file);
    
    printf("\n=== EXECUTING: cat << EOT | grep -v \"^#\" | sort > config.txt ===\n");
    printf("Enter config text with comments (lines starting with #).\n");
    printf("(type EOT on a line by itself to finish):\n");
    return redir;
}

// Helper function to free the tree
void free_tree(t_tree *node)
{
    if (!node) return;
    
    free_tree(node->left);
    free_tree(node->right);
    
    if (node->args) {
        int i = 0;
        while (node->args[i]) {
            i++;
        }
        free(node->args);
    }
    
    free(node);
}
int main(void)
{
    printf("\nChoose a complex command to test (1-6):\n");
    printf("1. cat /etc/passwd | grep root | wc -l > root_count.txt\n");
    printf("2. cat << EOF | sort | uniq -c > sorted_unique.txt\n");
    printf("3. ls -la | grep ^d | wc -l > dir_count.txt\n");
    printf("4. cat << MARKER | grep test | sort -r | head -n 2 > top_results.txt\n");
    printf("5. find . -type f | grep \".c$\" | sort | xargs wc -l | tail -n 1 > total_lines.txt\n");
    printf("6. cat << EOT | grep -v \"^#\" | sort > config.txt\n");
    
    char *input = readline("\nEnter choice: ");
    int choice = atoi(input);
    free(input);
    
    t_tree *command_tree = NULL;
    switch(choice) {
        case 1:
        command_tree = create_command1();
            break;
        case 2:
            command_tree = create_command2();
            break;
        case 3:
            command_tree = create_command3();
            break;
        case 4:
        command_tree = create_command4();
            break;
            case 5:
            command_tree = create_command5();
            break;
            case 6:
            command_tree = create_command6();
            break;
            default:
            printf("Invalid choice. Exiting.\n");
            return 1;
    }
        
    process_heredocs(command_tree);
    if (command_tree) {
        int status = execute_ast(command_tree);
        printf("\nCommand execution completed with status: %d\n", status);
        free_tree(command_tree);
    }
    
    return 0;
}