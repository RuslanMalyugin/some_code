#include <stdio.h>
#include <stdlib.h>
int
main (int arg_count, char *arg_var[])
{
  int y = 0;
  float x = 0.0;
  scanf ("%f%x", &x, &y);
  const char *z = arg_var[1];
  int zn = strtoul (z, NULL, 27);
  printf ("%.3f", x + y + zn);
  return 0;
}