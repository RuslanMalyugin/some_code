
#include <stdio.h>
#include <string.h>
int
main (int arg_count, char *arg_var[])
{
  char zero = '0';
  char x = 'x';
  for (int i = 1; i < arg_count; i++)
    {
      int counter = 0;
      for (int j = 0; j < strlen (arg_var[i]); j++)
        {
          if (arg_var[i][j] != zero && arg_var[i][j] != x)
            counter++;
        }
      if (counter == 0)
        counter = 1;
      counter = (counter + 1) / 2;
      printf ("%d ", counter);
    }
  return 0;
}