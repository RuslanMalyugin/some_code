#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

volatile int counter = 0;
volatile int chk_exit = 0;

static void
handler (int signum)
{
  if (signum == SIGINT)
    {
      ++counter;
    }
  else if (signum == SIGTERM)
  
    {
      chk_exit = 1;
      static char buffer[128];
      int size = snprintf (buffer, sizeof (buffer), "%d\n", counter);
      write (1, buffer, size);
    }
}

int
main ()
{
  sigset_t mask;
  sigfillset (&mask);
  sigprocmask (SIG_BLOCK, &mask, NULL);
  int signals[] = { SIGTERM, SIGINT, 0 };
  for (int *signal = signals; *signal; ++signal)
    {
      sigaction (
          *signal,
          &(struct sigaction){ .sa_handler = handler, .sa_flags = SA_RESTART },
          NULL);
    }

  printf ("%d\n", getpid ());
  fflush (stdout);
  sigemptyset (&mask);
  while (chk_exit != 1)
    {
      sigsuspend (&mask);
    }
  return 0;
}