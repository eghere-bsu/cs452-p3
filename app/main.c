#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../src/lab.h"

// Function to set up signal handlers for the parent shell
void setup_signal_handlers()
{
  // Ignore signals in the parent process (shell)
  signal(SIGINT, SIG_IGN);  // Ignore Ctrl+C (SIGINT)
  signal(SIGQUIT, SIG_IGN); // Ignore Ctrl+\ (SIGQUIT)
  signal(SIGTSTP, SIG_IGN); // Ignore Ctrl+Z (SIGTSTP)
  signal(SIGTTIN, SIG_IGN); // Ignore SIGTTIN (background input)
  signal(SIGTTOU, SIG_IGN); // Ignore SIGTTOU (background output)
}

int main(int argc, char *argv[])
{
  struct shell sh;

  // Pre-main loop setup
  sh_init(&sh);
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
    check_background_jobs();

    if (line && *line)
    {
      char **argv = cmd_parse(line, &sh);

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
          // In parent process
          if (sh.run_in_background)
          {
            if (job_count < MAX_JOBS)
            {
              jobs[job_count].pid = pid;
              strcpy(jobs[job_count].command, line);
              job_count++;
              printf("[Running in background] PID: %d\n", pid);
            }
            else
            {
              printf("Max jobs reached. Cannot add new job.\n");
            }
          }
          else
          {
            // Wait for foreground process to complete
            int status;
            waitpid(pid, &status, 0);
          }
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
