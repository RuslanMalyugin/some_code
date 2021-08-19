#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int
main ()
{
  size_t counter = 1;
  while (1)
    {
      int pid = fork ();
      if (pid < 0)
        {
          printf ("%ld", counter);
          return 0;
        }
      if (pid != 0)
        {
          int status;
          assert (waitpid (pid, &status, 0) != -1);
          break;
        }
      counter++;
    }
}