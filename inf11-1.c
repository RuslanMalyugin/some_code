#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  int N = strtoll (argv[1], NULL, 10);
  size_t counter = 1;
  printf ("%d", 1);
  for (int i = 1; i < N; ++i)
    {
      printf (" %d", i + 1);
      fflush (stdout);
      int pid = fork ();
      if (pid != 0)
        {
          int w = waitpid (pid, NULL, 0);
          return 0;
        }
    }
  printf ("%c", '\n');
  return 0;
}