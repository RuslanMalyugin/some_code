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
main ()
{
  sigset_t mask;
  sigfillset (&mask);
  sigdelset (&mask, SIGINT);
  sigprocmask (SIG_BLOCK, &mask, NULL);
  siginfo_t info;
  int pid;
  scanf ("%d", &pid);
  sigval_t received_value;
  received_value.sival_int = 1;
  while (received_value.sival_int > 0)
    {
      int value;
      scanf ("%d", &value);
      sigqueue (pid, SIGRTMIN, (union sigval){ .sival_int = value });
      sigwaitinfo (&mask, &info);
      int received_signal = info.si_signo;
      int received_value = info.si_value.sival_int;
      int received_pid = info.si_pid;
      if (received_signal == SIGINT)
        return 0;
      printf ("%d\n", received_value);
    }
  return 0;
}