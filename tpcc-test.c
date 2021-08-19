#include <stdio.h>
struct tmp
{
  int a;
  int b;
};
int
main ()
{
  struct tmp a;
  a.a = 1;
  a.b = 19;
  int c = a.a + a.b;
  printf ("%d", c);
  return 0;
}