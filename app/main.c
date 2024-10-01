#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../src/lab.h"

int main(int argc, char *argv[])
{
  struct shell sh;

  // Pre-main loop setup
  sh_init(&sh);
  parse_args(argc, argv);
  using_history();

  char *prompt = sh.prompt;
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
        pid_t pid = fork();
        if (pid < 0)
        {
          perror("fork");
        }
        else if (pid == 0)
        {
          // Child process: reset signals to default behavior
          signal(SIGINT, SIG_DFL);
          signal(SIGQUIT, SIG_DFL);
          signal(SIGTSTP, SIG_DFL);
          signal(SIGTTIN, SIG_DFL);
          signal(SIGTTOU, SIG_DFL);

          // Execute the external command
          execvp(argv[0], argv);
          // If execvp fails
          perror("execvp");
          exit(EXIT_FAILURE);
        }
        else
        {
          // Parent process: wait for the child to finish
          int status;
          waitpid(pid, &status, 0);
        }
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