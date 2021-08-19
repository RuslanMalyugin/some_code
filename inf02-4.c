#include <stdint.h>
#include <stdio.h>
char
symbol_coding (int input)
{
  if (input >= '0' && input <= '9')
    return (input - '0');
  if (input >= 'a' && input <= 'z')
    return (input - 'a' + 10 + 26);
  if (input >= 'A' && input <= 'Z')
    return (input - 'A' + 10);
  return 127;
}
char
symbol_decoding (int input)
{
  if (input >= 0 && input < 10)
    return (input + '0');
  if (input >= 36 && input < 62)
    return (input + 'a' - 10 - 26);
  if (input >= 10 && input < 36)
    return (input + 'A' - 10);
  return 127;
}

int
main ()
{
  __uint64_t buffer = 0;
  __uint64_t new_set = 0;
  char c = getchar ();
  while (c != '\n' && c != EOF)
    {
      if ((c != '&') && (c != '~') && (c != '|') && (c != '^'))
        new_set |= (1ull << symbol_coding (c));
      if (c == '&')
        {
          buffer &= new_set;
          new_set = 0;
        }
      if (c == '|')
        {
          buffer |= new_set;
          new_set = 0;
        }
      if (c == '^')
        {
          buffer ^= new_set;
          new_set = 0;
        }
      if (c == '~')
        {
          buffer = ~buffer;
          new_set = 0;
        }
      c = getchar ();
    }
  for (int i = 0; i < 62; i++)
    {
      if (((buffer >> i) & 1ull) == 1ull)
        printf ("%c", (char)symbol_decoding (i));
    }
  return 0;
}