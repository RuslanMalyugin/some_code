#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

volatile int counter = 0;
volatile int chk_exit = 0;

static void
handler (int signum)
{
  if (signum == SIGINT || signum == SIGTERM)
    {
      chk_exit = 1;
    }
  else if (signum == SIGUSR1 || signum == SIGUSR2)
    {
      static char buffer[128];
      if (signum == SIGUSR1)
        {
          ++counter;
        }
      else if (signum == SIGUSR2)
        {
          counter *= (-1);
        }
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
  int start_num;
  int signals[] = { SIGUSR1, SIGUSR2, SIGTERM, SIGINT, 0 };
  for (int *signal = signals; *signal; ++signal)
    {
      sigaction (
          *signal,
          &(struct sigaction){ .sa_handler = handler, .sa_flags = SA_RESTART },
          NULL);
    }
  printf ("%d\n", getpid ());
  fflush (stdout);
  scanf ("%d", &start_num);
  counter = start_num;
  sigemptyset (&mask);
  while (chk_exit != 1)
    {
      sigsuspend (&mask);
    }
  return 0;
}