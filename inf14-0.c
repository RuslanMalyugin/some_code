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
  sigset_t full_mask;
  sigfillset (&full_mask);
  sigprocmask (SIG_BLOCK, &full_mask, NULL);
  while (1)
    {
      siginfo_t info;
      sigwaitinfo (&full_mask, &info);
      int received_signal = info.si_signo;
      if (received_signal == SIGRTMIN)
        {
          return 0;
        }
      if (SIGRTMIN < received_signal && received_signal <= SIGRTMAX)
        {
          char first_string[4096];
          char *file_name = argv[received_signal - SIGRTMIN];
          FILE *fp;
          fp = fopen (file_name, "r");
          fgets (first_string, sizeof (first_string), fp);
          int size = 0;
          while (first_string[size] != '\n')
            {
              ++size;
            }
          first_string[size] = '\n';
          write (1, first_string, size + 1);
          fclose (fp);
        }
    }
  return 0;
}