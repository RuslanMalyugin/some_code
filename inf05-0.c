#include <stdio.h>
void summ ();

int
main ()
{
  int N = 3;
  int A_t[N];
  int B_t[N];
  int R_t[N];
  for (int i = 0; i < N; i++)
    {
      A_t[i] = 10;
      B_t[i] = 2;
      R_t[i] = 0;
    }
  summ (N, A_t, B_t, R_t);
  for (int i = 0; i < N; i++)
    {
      printf ("%d \n", R_t[i]);
    }
  return 0;
}