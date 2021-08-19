#include <stdint.h>
#include <stdio.h>
uint16_t
satsum (uint16_t x, uint16_t y)
{
  uint16_t a = x + y;
  return ((a < x) || (a < y)) ? (-1) : (a);
}
// int
// main ()
// {
//   uint16_t a, b;
//   scanf ("%hd %hd", &a, &b);
//   uint16_t res = satsum (a, b);
//   printf ("%d", res);
// }