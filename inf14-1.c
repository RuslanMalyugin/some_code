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
  printf ("%d\n", getpid ());
  sigset_t mask;
  sigfillset (&mask);
  sigprocmask (SIG_BLOCK, &mask, NULL);
  siginfo_t info;
  sigval_t received_value;
  received_value.sival_int = 1;
  while (received_value.sival_int > 0)
    {
      sigwaitinfo (&mask, &info);
      int received_signal = info.si_signo;
      received_value = info.si_value;
      int received_pid = info.si_pid;
      sigqueue (received_pid, SIGRTMIN,
                (union sigval){ .sival_int = received_value.sival_int - 1 });
    }
  return 0;
}