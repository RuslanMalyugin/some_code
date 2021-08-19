#include <stdint.h>
#include <stdio.h>
int
checker (int slides, int *ASCII_counter, int *not_ASCII_counter)
{
  for (int i = 0; i <= slides; i++)
    {
      int c = getchar ();
      if ((c >> 6) == 2)
        continue;
      else
        {
          printf ("%d %d", *ASCII_counter, *not_ASCII_counter);
          return 1;
        }
    }
  *not_ASCII_counter = *not_ASCII_counter + 1;
  return 0;
}
int
main ()
{
  int c = getchar ();
  int ASCII_counter = 0;
  int not_ASCII_counter = 0;
  while (c != EOF)
    {
      if ((c >> 7) == 0)
        ASCII_counter++;
      else if ((c >> 5) == 6)
        {
          if (checker (0, &ASCII_counter, &not_ASCII_counter) == 1)
            return 1;
        }
      else if ((c >> 4) == 14)
        {
          if (checker (1, &ASCII_counter, &not_ASCII_counter) == 1)
            return 1;
        }
      else if ((c >> 3) == 30)
        {
          if (checker (2, &ASCII_counter, &not_ASCII_counter) == 1)
            return 1;
        }
      else
        {
          printf ("%d %d", ASCII_counter, not_ASCII_counter);
          return 1;
        }
      c = getchar ();
    }
  printf ("%d %d", ASCII_counter, not_ASCII_counter);
  return 0;
}
