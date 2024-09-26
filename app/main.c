#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../src/lab.h"

void print_version()
{
  printf("Shell version: %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
}

int main(int argc, char *argv[])
{
  int opt;
  struct shell sh;
  
  // Parse command-line arguments
  while ((opt = getopt(argc, argv, "v")) != -1)
  {
    switch (opt)
    {
    case 'v':
      print_version();
      return 0; // Exit after printing version
    default:
      printf("Usage: %s [-v]\n", argv[0]);
      return 1; // Exit with error for incorrect usage
    }
  }

  using_history();

  char *prompt = get_prompt("MY_PROMPT");
  if (prompt == NULL)
  {
    printf("Failed to get the shell prompt.\n");
    return 1;
  }

  char *line;
  while ((line = readline(prompt)))
  {
    trim_white(line);

    if (line && *line)
    {
      // Parse the command into arguments
      char **argv = cmd_parse(line);

      // Check if the command is a built-in
      if (!do_builtin(&sh, argv))
      {
        // If not a built-in, handle it as an external command here
        printf("Executing external command: %s\n", line);
        // Here you'd typically fork and execvp to run the command.
      }

      // Clean up
      cmd_free(argv);
      add_history(line);
    }

    printf("You entered: %s\n", line);

    free(line);
  }

  free(prompt);

  return 0;
}
