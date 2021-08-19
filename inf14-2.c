#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

volatile int child_checker = 0;
volatile int alarm_checker = 0;

void
handler (int signal)
{
  if (signal == SIGALRM)
    {
      if (child_checker == 0)
        alarm_checker = 1;
    }
  else if (signal == SIGCHLD)
    {
      child_checker = 1;
    }
}

int
main (int argc, char *argv[])
{
  sigaction (
      SIGCHLD,
      &(struct sigaction){ .sa_handler = handler, .sa_flags = SA_RESTART },
      NULL);

  int live_time_sec = strtol (argv[1], NULL, 10);
  char *file_name = argv[2];
  sigset_t full_mask;
  sigemptyset (&full_mask);
  sigaddset (&full_mask, SIGALRM);
  sigaddset (&full_mask, SIGCHLD);
  sigprocmask (SIG_BLOCK, &full_mask, NULL);
  pid_t pid = fork ();
  if (pid == 0)
    {
      sigfillset (&full_mask);
      sigprocmask (SIG_UNBLOCK, &full_mask, NULL);
      execvp (file_name, argv + 2);
    }
  else
    {
      /*
      //For debug:

      printf ("Child: %d\n", pid);
      printf ("Process: %d\n", getpid ());
      */
      int signals[] = { SIGALRM, SIGCHLD, 0 };
      for (int *signal = signals; *signal; ++signal)
        {

          sigaction (*signal,
                     &(struct sigaction){ .sa_handler = handler,
                                          .sa_flags = SA_RESTART,
                                          .sa_mask = full_mask },
                     NULL);
        }
      alarm (live_time_sec);
      sigemptyset (&full_mask);
      sigsuspend (&full_mask);
      if (alarm_checker == 1)
        {
          printf ("%s", "timeout");
          fflush (stdout);
          kill (pid, SIGTERM);
          waitpid (pid, NULL, 0);
          return 2;
        }
      else
        {
          int status;
          waitpid (pid, &status, 0);

          if (WIFSIGNALED (status) != 0)
            {
              printf ("%s", "signaled");
              fflush (stdout);
              return 1;
            }
          else
            {
              printf ("%s", "ok");
              fflush (stdout);
              return 0;
            }
        }
    }
  return 0;
}
