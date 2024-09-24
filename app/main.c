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

  // Get the prompt from the environment variable MY_PROMPT
  const char *prompt = getenv("MY_PROMPT");
  if (prompt == NULL)
  {
    printf("MY_PROMPT env is not set.\n");
    return 1;
  }

  char *line;
  while ((line = readline(prompt)))
  {
    if (line && *line)
    {
      add_history(line);
    }

    printf("You entered: %s\n", line);

    free(line); // Free memory allocated by readline
  }

  return 0;
}
