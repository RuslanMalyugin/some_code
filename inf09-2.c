#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
main ()
{
  char path[PATH_MAX];
  char *n;
  char buffer[PATH_MAX + 3];
  while (fgets (path, sizeof (path), stdin))
    {
      n = memchr (path, '\n', sizeof (path));
      if (n)
        {
          *n = '\0';
        }
      if (access (path, X_OK) == 0)
        {
          FILE *file = fopen (path, "r");
          fgets (buffer, sizeof (buffer), file);
          buffer[strlen (buffer) - 1] = '\0';
          int check = 1;
          if (buffer[0] == 0x7f && buffer[1] == 'E' && buffer[2] == 'L'
              && buffer[3] == 'F')
            check = 0;
          if (buffer[0] == '#' && buffer[1] == '!')
            if (access (buffer + 2, X_OK) == 0)
              check = 0;
          if (check == 1)
            printf ("%s", path);
        }
    }
  return 0;
}