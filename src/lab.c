#include "lab.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

/**
 * @brief Set the shell prompt. This function will attempt to load a prompt
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
    // TODO: Implement this function
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
    // TODO: Implement this function
}

/**
 * @brief Free the line that was constructed with cmd_parse
 *
 * @param line the line to free
 */
void cmd_free(char **line)
{
    // TODO: Implement this function
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
    // TODO: Implement this function
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
    // TODO: Implement this function
}
