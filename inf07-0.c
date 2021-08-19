#include <sys/syscall.h>
long syscall (long number, ...);

void
_start ()
{
  char text[] = "Hello, World!";
  int length = sizeof (text) - 1;
  syscall (SYS_write, 1, text, length);
  syscall (SYS_exit, 0);
}
