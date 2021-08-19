#include <stdio.h>
struct test_son
{
  int a;
  long long b;
};
struct test
{
  // int a[2];
  // struct test_son son;
  char b;
  char c;
};

int
main ()
{
  struct test d;
  printf ("%d", sizeof (d));
  return 0;
}