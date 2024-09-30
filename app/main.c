#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../src/lab.h"

int main(int argc, char *argv[])
{
  struct shell sh;
  
  parse_args(argc, argv);

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
      char **argv = cmd_parse(line);

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
    //For debugging purposes
    //printf("You entered: %s\n", line);

    free(line);
  }

  // free(prompt);
  sh_destroy(&sh);
  return 0;
}
