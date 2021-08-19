#include <stdio.h>
int summ (int x0, int N, int *X);

int
main ()
{
  int arr[4] = { 1, 1, 1, 1 };
  int res = summ (1, 4, arr);
  printf ("%d", res);
}