#include <stdio.h>
double calc (double A, double B, double C, int D);

void vector_sum (int N, const float *A, const float *B, float *R);

int
main ()
{
  int N = 4;
  float A[4] = { 1.1, 1.1, 1.1, 1.1 };
  float B[4] = { 1.1, 1.1, 1.1, 1.1 };
  float C[4];
  vector_sum (N, A, B, C);
  for (int i = 0; i < N; i++)
    {
      printf ("%f \n", C[i]);
    }
  float res = calc (1.5, 1.5, 0.5, 1);
  printf ("%lf", res);
  return 0;
}