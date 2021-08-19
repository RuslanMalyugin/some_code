#include <stdio.h>

int solve (int A, int B, int C, int D);
// int
// solve (int A, int B, int C, int D)
// {
//   if (D == 0)
//     return 0;
//   int D_pos = D;
//   int divider = 1;
//   if (D < 0)
//     D_pos *= -1;
//   for (int i = 0; i <= D_pos; i++)
//     {
//       int ans = A;
//       ans = B + ans * divider;
//       ans = C + ans * divider;
//       ans = D + ans * divider;
//       if (ans == 0)
//         return divider;
//       do
//         divider++;
//       while (D % divider != 0 && divider <= D_pos);
//     }
//   return divider;
// }
int
main ()
{
  int A, B, C, D;
  scanf ("%d %d %d %d", &A, &B, &C, &D);
  int res = solve (A, B, C, D);
  printf ("%d", res);
}