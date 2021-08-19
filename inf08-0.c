#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  assert (argc >= 3);
  const char *file_name = argv[1];
  int fd = open (file_name, O_RDONLY);
  if (fd == -1)
    return 1;
  file_name = argv[2];
  int first_file = open (file_name, O_CREAT | O_WRONLY | O_TRUNC, 0666);
  file_name = argv[3];
  int second_file = open (file_name, O_CREAT | O_WRONLY | O_TRUNC, 0666);
  if (first_file == -1 || second_file == -1)
    {
      return 2;
    }
  char buffer[4096];
  char numbers[4096];
  char not_numbers[4096];
  long long bytes_read = 1;
  long long bytes_write = 0;
  size_t num_counter = 0;
  size_t not_num_counter = 0;
  while (bytes_read > 0)
    {
      bytes_read = read (fd, buffer, sizeof (buffer));
      for (size_t i = 0; i < bytes_read; ++i)
        {
          if (bytes_read == -1)
            {
              return 3;
            }
          if (buffer[i] <= '9' && '0' <= buffer[i])
            {
              numbers[num_counter] = buffer[i];
              num_counter++;
            }
          else
            {
              not_numbers[not_num_counter] = buffer[i];
              not_num_counter++;
            }
        }
    }
  bytes_write = write (first_file, numbers, num_counter);
  bytes_write = write (second_file, not_numbers, not_num_counter);
  if (bytes_write == -1)
    {
      return 3;
    }
  return 0;
}