#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
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
  struct Item current[1];
  size_t size_struct = sizeof (current[0]);
  const char *file_name = argv[1];
  int fd = open (file_name, O_RDONLY);
  size_t ptr = 0;
  if (lseek (fd, 0, SEEK_END) / size_struct == 0)
    return 0;
  do
    {
      lseek (fd, ptr, SEEK_SET);
      read (fd, current, size_struct);
      printf ("%d ", current[0].value);
      ptr = current[0].next_pointer;
    }
  while (ptr != 0);
  return 0;
}