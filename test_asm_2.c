#include <stdio.h>
int summ (int x0, int N, int *X);
int
main ()
{
  int sum[3];
  for (int i = 0; i < 3; i++)
    {
      sum[i] = 10;
    }
  printf ("%d", summ (-10, 3, sum));
}