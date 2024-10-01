#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../src/lab.h"

// Function to set up signal handlers for the parent shell
void setup_signal_handlers() {
    // Ignore signals in the parent process (shell)
    signal(SIGINT, SIG_IGN);    // Ignore Ctrl+C (SIGINT)
    signal(SIGQUIT, SIG_IGN);   // Ignore Ctrl+\ (SIGQUIT)
    signal(SIGTSTP, SIG_IGN);   // Ignore Ctrl+Z (SIGTSTP)
    signal(SIGTTIN, SIG_IGN);   // Ignore SIGTTIN (background input)
    signal(SIGTTOU, SIG_IGN);   // Ignore SIGTTOU (background output)
}

int main(int argc, char *argv[])
{
  struct shell sh;
  
  // Pre-main loop setup
  setup_signal_handlers();
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

    free(line);
  }

  sh_destroy(&sh);
  return 0;
}
