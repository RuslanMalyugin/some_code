#include <sys/syscall.h>
long syscall (long number, ...);

void
_start ()
{
  int buffer_size = 1;
  char buffer[buffer_size];
  while (syscall (SYS_read, 0, buffer, buffer_size))
    {
      syscall (SYS_write, 1, buffer, buffer_size);
    }
  syscall (SYS_exit, 0);
}