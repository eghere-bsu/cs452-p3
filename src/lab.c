#include "lab.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

// Function prototypes for built-in commands
static bool handle_exit(struct shell *sh, char **argv);
static bool handle_cd(struct shell *sh, char **argv);
static bool handle_ls(struct shell *sh, char **argv);
static bool handle_history(struct shell *sh, char **argv);

// Define structures for built-in commands
typedef struct
{
    const char *name;
    bool (*func)(struct shell *sh, char **argv);
} builtin_command;

static const builtin_command builtins[] = {
    {"exit", handle_exit},
    {"cd", handle_cd},
    {"ls", handle_ls},
    {"history", handle_history}
};

// Number of built-in commands
static const size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);

/**
 * @brief Handle the 'exit' command. This function will exit the shell.
 *
 * @param sh The shell
 * @param argv The command arguments
 * @return True since 'exit' is a built-in command
 */
static bool handle_exit(struct shell *sh, char **argv)
{
    UNUSED(argv);
    sh_destroy(sh); // Clean up shell resources
    exit(0);        // Exit the program
    return true;    // Should never reach here
}

/**
 * @brief Handle the 'cd' command. This function will change the current directory.
 *
 * @param sh The shell
 * @param argv The command arguments (argv[1] is the directory to change to)
 * @return True since 'cd' is a built-in command
 */
static bool handle_cd(struct shell *sh, char **argv)
{
    UNUSED(sh);
    const char *dir = argv[1]; // Directory to change to

    if (dir == NULL || strcmp(dir, "~") == 0)
    {
        // No directory specified, or '~' specified: change to home directory
        dir = getenv("HOME");
    }

    if (chdir(dir) != 0)
    {
        // Error changing directory
        perror("cd");
    }

    return true;
}

/**
 * @brief Handle the 'ls' command. This function will list directory contents.
 *
 * @param sh The shell
 * @param argv The command arguments (e.g., 'ls -l')
 * @return True since 'ls' is a built-in command
 */
static bool handle_ls(struct shell *sh, char **argv)
{
    UNUSED(sh);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return true; // Fork failed, return true since 'ls' is a built-in command
    }
    else if (pid == 0)
    {
        // Child process: execute 'ls' command
        execvp("ls", argv);
        // If execvp fails:
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process: wait for child to finish
        int status;
        waitpid(pid, &status, 0);
    }

    return true;
}

/**
 * @brief Handle the 'history' command. This function will print the command history.
 *
 * @param sh The shell
 * @param argv The command arguments
 * @return True since 'history' is a built-in command
 */
static bool handle_history(struct shell *sh, char **argv)
{
    UNUSED(sh);
    UNUSED(argv);

    // Get the history list
    HIST_ENTRY **history = history_list();
    if (history)
    {
        // Print each entry in the history
        for (int i = 0; history[i]; i++)
        {
            printf("%d: %s\n", i + history_base, history[i]->line);
        }
    }
    else
    {
        printf("No history available.\n");
    }

    return true;
}

/**
 * @brief Get the shell prompt. This function will attempt to load a prompt
 * from the requested environment variable, if the environment variable is
 * not set a default prompt of "shell>" is returned.  This function calls
 * malloc internally and the caller must free the resulting string.
 *
 * @param env The environment variable
 * @return const char* The prompt
 */
char *get_prompt(const char *env)
{
    // Get the prompt from the environment variable
    const char *prompt_env = getenv(env);

    // If the environment variable is not set or empty, use the default prompt
    const char *default_prompt = "shell>";
    const char *prompt = (prompt_env && strlen(prompt_env) > 0) ? prompt_env : default_prompt;

    // Allocate memory for the prompt string
    char *prompt_copy = (char *)malloc(strlen(prompt) + 1); // +1 for null terminator
    if (!prompt_copy)
    {
        perror("Failed to allocate memory for the prompt");
        return NULL;
    }

    // Copy the prompt into the allocated memory
    strcpy(prompt_copy, prompt);

    return prompt_copy; // Caller must free this memory
}

/**
 * Changes the current working directory of the shell. Uses the linux system
 * call chdir. With no arguments the users home directory is used as the
 * directory to change to.
 *
 * @param dir The directory to change to
 * @return  On success, zero is returned.  On error, -1 is returned, and
 * errno is set to indicate the error.
 */
int change_dir(char **dir)
{
    struct shell sh;
    handle_cd(&sh, dir);
    return 0;
}

/**
 * @brief Convert line read from the user into to format that will work with
 * execvp. We limit the number of arguments to ARG_MAX loaded from sysconf.
 * This function allocates memory that must be reclaimed with the cmd_free
 * function.
 *
 * @param line The line to process
 *
 * @return The line read in a format suitable for exec
 */
char **cmd_parse(char const *line)
{
    if (line == NULL)
    {
        return NULL;
    }

    // Get the maximum number of arguments
    long arg_max = sysconf(_SC_ARG_MAX);
    if (arg_max == -1)
    {
        arg_max = 4096; // Fallback value if sysconf fails
    }

    // Allocate memory for argument list (limit to ARG_MAX)
    char **argv = malloc((arg_max + 1) * sizeof(char *));
    if (!argv)
    {
        perror("malloc");
        return NULL;
    }

    // Tokenize the input line
    char *line_copy = strdup(line); // Copy the line
    if (!line_copy)
    {
        perror("strdup");
        free(argv);
        return NULL;
    }

    int argc = 0;
    char *token = strtok(line_copy, " \t\n"); // Split by spaces, tabs, or newlines
    while (token != NULL && argc < arg_max)
    {
        argv[argc++] = strdup(token); // Copy each token to the argv array
        token = strtok(NULL, " \t\n");
    }

    argv[argc] = NULL; // Null-terminate the array

    free(line_copy); // Free the temporary line copy

    return argv;
}

/**
 * @brief Free the line that was constructed with cmd_parse
 *
 * @param line the line to free
 */
void cmd_free(char **line)
{
    if (line == NULL)
    {
        return;
    }

    // Free each argument string
    for (int i = 0; line[i] != NULL; i++)
    {
        free(line[i]);
    }

    free(line); // Free the argument list itself
}

/**
 * @brief Trim the whitespace from the start and end of a string.
 * For example "   ls -a   " becomes "ls -a". This function modifies
 * the argument line so that all printable chars are moved to the
 * front of the string
 *
 * @param line The line to trim
 * @return The new line with no whitespace
 */
char *trim_white(char *line)
{
    // Check for NULL input
    if (line == NULL)
    {
        return NULL;
    }

    // Trim leading whitespace
    char *start = line;
    while (isspace((unsigned char)*start))
    {
        start++;
    }

    // Trim trailing whitespace
    char *end = line + strlen(line) - 1;
    while (end > start && isspace((unsigned char)*end))
    {
        end--;
    }

    // Null terminate the trimmed string
    *(end + 1) = '\0';

    // Move the trimmed string to the start of the original line
    if (start != line)
    {
        memmove(line, start, end - start + 2); // +2 to include null terminator
    }

    return line; // Return the trimmed string
}

/**
 * @brief Takes an argument list and checks if the first argument is a
 * built in command such as exit, cd, jobs, etc. If the command is a
 * built in command this function will handle the command and then return
 * true. If the first argument is NOT a built in command this function will
 * return false.
 *
 * @param sh The shell
 * @param argv The command to check
 * @return True if the command was a built in command
 */
bool do_builtin(struct shell *sh, char **argv)
{
    if (argv == NULL || argv[0] == NULL)
    {
        return false;
    }

    for (size_t i = 0; i < num_builtins; i++)
    {
        if (strcmp(argv[0], builtins[i].name) == 0)
        {
            return builtins[i].func(sh, argv); // Execute the built-in command
        }
    }

    return false; // Command is not a built-in
}

/**
 * @brief Initialize the shell for use. Allocate all data structures
 * Grab control of the terminal and put the shell in its own
 * process group. NOTE: This function will block until the shell is
 * in its own program group. Attaching a debugger will always cause
 * this function to fail because the debugger maintains control of
 * the subprocess it is debugging.
 *
 * @param sh
 */
void sh_init(struct shell *sh)
{
    // TODO: Implement this function 
}

/**
 * @brief Destroy shell. Free any allocated memory and resources and exit
 * normally.
 *
 * @param sh
 */
void sh_destroy(struct shell *sh)
{
    // TODO: Implement this function
}

/**
 * @brief Parse command line args from the user when the shell was launched
 *
 * @param argc Number of args
 * @param argv The arg array
 */
void parse_args(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "v")) != -1)
    {
        switch (opt)
        {
        case 'v':
            printf("Shell version: %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
            exit(0); // Exit after printing version
        default:
            printf("Usage: %s [-v]\n", argv[0]);
            exit(1); // Exit with error for incorrect usage
        }
    }
}
