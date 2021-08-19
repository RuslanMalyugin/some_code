#define _GNU_SOURCE
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  int fd = open (argv[1], O_RDONLY);
  struct stat s;
  assert (fstat (fd, &s) == 0);
  void *mapped = mmap (NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  char *buf = mapped;
  size_t pattern_size = strlen (argv[2]);
  size_t file_size = s.st_size;
  size_t slide = 0;
  size_t pos = 0;
  char *curr_pos;
  do
    {
      curr_pos
          = memmem (buf + slide, file_size - slide, argv[2], pattern_size);
      if (curr_pos != NULL)
        {
          pos = curr_pos - buf;
          slide = pos + 1;
          printf ("%ld ", pos);
        }
    }
  while (curr_pos != NULL);
  close (fd);
  munmap (mapped, s.st_size);
  return 0;
}