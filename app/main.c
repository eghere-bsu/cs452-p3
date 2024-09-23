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
    default:
      printf("Usage: %s [-v]\n", argv[0]);
    }
  }

  // Initialize history handling for GNU Readline
  using_history();

  char *line;
  while ((line = readline("$ ")))
  { // Display "$ " as the prompt
    if (line && *line)
    { // Only add non-empty commands to history
      add_history(line);
    }

    printf("You entered: %s\n", line); // Just a placeholder for command execution

    free(line); // Free memory allocated by readline
  }

  return 0;
}
