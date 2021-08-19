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

// size_t
// char_to_int (char *char_num, size_t size)
// {
//   size_t num = 0;
//   size_t pow_10 = 1;
//   for (size_t i = 0; i < size; ++i)
//     {
//       num += ((char_num[size - i - 1] - '0') * pow_10);
//       pow_10 *= 10;
//     }
//   return num;
// }

int
main (int argc, char *argv[])
{
  int fd = open (argv[1], O_RDWR | O_CREAT | O_TRUNC); // opening
  long long width = 0;
  long long height = 0;
  height = strtoll (argv[2], NULL, 10);
  width = height;
  long long format_len = strtoll (argv[3], NULL, 10);
  size_t size = height * width * format_len + height;
  ftruncate (fd, size);
  long long curr = 1;
  int64_t **array = (int64_t **)calloc (height, sizeof (int64_t *));
  for (size_t i = 0; i < height; ++i)
    {
      array[i] = (int64_t *)calloc (width, sizeof (int64_t));
    }
  long long c_w = 0;
  long long c_h = 0;
  while (c_h < height / 2 && c_w < width / 2)
    {
      for (long long i = c_w; i < width - c_w; i++)
        {
          array[c_h][i] = curr;
          ++curr;
        }
      for (long long i = 1; i < (height - 2 * c_h); i++)
        {
          array[c_h + i][width - c_w - 1] = curr;
          ++curr;
        }
      for (long long i = width - 2 - c_w; i >= c_w; --i)
        {
          array[height - c_h - 1][i] = curr;
          ++curr;
        }
      for (long long i = height - c_h - 2; i > c_h; --i)
        {
          array[i][c_w] = curr;
          ++curr;
        }
      ++c_h;
      ++c_w;
    }
  curr = width * height;
  if ((height % 2 == 1) || (width % 2 == 1))
    {
      if (height > width)
        {
          for (long long i = height - width / 2 - 1; i >= width / 2; --i)
            {
              array[i][width / 2] = curr;
              --curr;
            }
        }
      else
        {
          for (long long i = width - height / 2 - 1; i >= height / 2; --i)
            {
              array[height / 2][i] = curr;
              --curr;
            }
        }
    }

  void *mapped = mmap (
      /* desired addr, addr = */ NULL,
      /* length = */ size,
      /* access attributes, prot = */ PROT_READ | PROT_WRITE,
      /* flags = */ MAP_SHARED,
      /* fd = */ fd,
      /* offset in file, offset = */ 0);
  char *buf = mapped;
  for (long long i = 0; i < height; i++)
    {
      for (long long j = 0; j < width; j++)
        {
          size_t num = array[i][j];
          for (long long k = format_len - 1; k >= 0; k--)
            {
              if (num / 10 != 0 || num % 10 != 0)
                {
                  buf[i * (height * format_len + 1) + j * format_len + k]
                      = (num % 10) + '0';
                  num /= 10;
                }
              else
                buf[i * (height * format_len + 1) + j * format_len + k] = ' ';
            }
        }
      buf[(i + 1) * (height * format_len + 1) - 1] = '\n';
    }
  munmap (mapped, size);
  close (fd);
  for (int64_t i = 0; i < height; ++i)
    {
      free (array[i]);
    }
  free (array);
  return 0;
}