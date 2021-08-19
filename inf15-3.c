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

int
main (int argc, char *argv[])
{
  if (argc == 1)
    {
      return 0;
    }
  else if (argc == 2)
    {
      pid_t pid = fork ();
      if (pid == 0)
        {
          execlp (argv[1], argv[1], NULL);
        }
      int status = 0;
      waitpid (pid, &status, 0);
      return 0;
    }
  else
    {
      int *fd = (int *)malloc (2 * (argc + 1) * sizeof (int));
      for (int i = 0; i < argc; ++i)
        {
          pipe2 (fd + 2 * i, O_CLOEXEC);
        }
      pid_t pids[argc];
      pids[1] = fork ();
      if (pids[1] == 0)
        {
          dup2 (*(fd + 3), 1);
          execlp (argv[1], argv[1], NULL);
        }
      else
        {
          for (int i = 2; i < argc - 1; ++i)
            {
              pids[i] = fork ();
              if (pids[i] == 0)
                {
                  dup2 (*(fd + 2 * i + 1), 1);
                  dup2 (*(fd + 2 * (i - 1)), 0);
                  execlp (argv[i], argv[i], NULL);
                }
            }
          pids[argc - 1] = fork ();
          if (pids[argc - 1] == 0)
            {
              dup2 (*(fd + argc * 2 - 4), 0);
              execlp (argv[argc - 1], argv[argc - 1], NULL);
            }
          else
            {
              for (int i = 0; i < argc * 2 + 1; ++i)
                {
                  close (*(fd + i));
                }
              for (int i = 1; i < argc; ++i)
                {
                  int status = 0;
                  waitpid (pids[i], &status, 0);
                }
              return 0;
            }
        }
    }
}