#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
struct Item
{
  int value;
  uint32_t next_pointer;
};

int
main (int argc, char *argv[])
{
  int fd = open (argv[1], O_RDONLY);
  struct stat s;
  assert (fstat (fd, &s) == 0);
  void *mapped = mmap (NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  char *buffer = mapped;
  size_t file_size = s.st_size;
  struct Item test;
  size_t struct_size = sizeof (test);

  if (file_size / struct_size == 0)
    return 0;

  size_t ptr = 0;
  struct Item *current = (struct Item *)buffer;
  do
    {
      printf ("%d ", current[ptr / 8].value);
      ptr = current[ptr / 8].next_pointer;
    }
  while (ptr != 0);
  close (fd);
  munmap (mapped, s.st_size);
  return 0;
}