#include <stdio.h>
#include <unistd.h>
int
main (int argc, char *argv[])
{
  int seconds = strtoll (argv[1], NULL, 10);
  sleep (seconds);
  printf ("%s", "Hello world!\n");
  return 0;
}