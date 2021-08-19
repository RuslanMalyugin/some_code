#include <stdio.h>
double my_sin (double x);
int
main ()
{
  double p;
  scanf ("%lf", &p);
  double res = my_sin (p);
  printf ("%lf", res);
  return 0;
}