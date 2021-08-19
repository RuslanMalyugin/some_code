int
summ (int x0, int N, int *X)
{
  for (int i = 0; i < N; ++i)
    {
      x0 += X[i];
    }
  return x0;
}