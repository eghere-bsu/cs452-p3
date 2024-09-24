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
      add_history(line);
    }

    printf("You entered: %s\n", line);

    free(line); 
  }
  
  free(prompt);

  return 0;
}
