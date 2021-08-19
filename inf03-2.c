int
solve (int A, int B, int C, int D)
{
  for (int divider = 0; divider < 255; divider++)
    {
      int ans = A;
      ans = ans * divider + B;
      ans = ans * divider + C;
      ans = ans * divider + D;
      if (ans == 0)
        {
          return divider;
        }
    }
  return 0;
}