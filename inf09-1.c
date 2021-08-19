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
  struct stat s;
  size_t byte_size = 0;
  char path[PATH_MAX];
  char *n;
  while (fgets (path, sizeof (path), stdin))
    {
      n = memchr (path, '\n', sizeof (path));
      if (n)
        {
          *n = '\0';
        }
      if (lstat (path, &s) != -1 && S_ISREG (s.st_mode))
        byte_size += s.st_size;
    }
  printf ("%ld", byte_size);
  return 0;
}