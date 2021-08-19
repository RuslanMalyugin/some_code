#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

int
main ()
{
  int fd = open ("output", O_RDWR | O_CREAT, 0664);
  write (fd, "GET txt1.txt HTTP/1.1\r\naa\r\n\r\n", 30 - 1);
  close (fd);
  return 0;
}