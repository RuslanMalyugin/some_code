#include <stdio.h>
int f (int a, int b, int c, int x);
int
main ()
{
  int a = f (1, 2, 3, 4);
  printf ("%d", a);
}
