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
  printf ("%d", getpid ());
  fflush (stdout);
  sigset_t full_mask;
  sigfillset (&full_mask);
  sigdelset (&full_mask, SIGINT);
  sigprocmask (SIG_BLOCK, &full_mask, NULL);

  char *FIFO_name = argv[1];
  int n = strtol (argv[2], 0, 10);

  int other_pid;
  scanf ("%d", &other_pid);

  kill (other_pid, SIGHUP);

  mkfifo (FIFO_name, 0644);
  int fd = open (FIFO_name, O_WRONLY);

  int count = 0;
  for (int i = 0; i <= n; ++i)
    {
      char buffer[4096];
      snprintf (buffer, 128, "%d ", i);
      write (fd, buffer, 128);
      // siginfo_t info;
      // sigwaitinfo(&full_mask, &info);

      //        int received_signal = info.si_signo;
      //        if (received_signal == SIGPIPE) {
      //            printf("%d", i);
      //            fflush(stdout);
      //        }
      count = i;
    }
  close (fd);
  printf ("%d", count);
  return 0;
}