
#include <arpa/inet.h>
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
main (int argc, char *argv[])
{
  struct in_addr ip_addr;
  inet_aton (argv[1], &ip_addr);
  long long port = strtoll (argv[2], NULL, 10);
  int socket_fd = socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port);
  addr.sin_addr = ip_addr;
  while (true)
    {
      int connect_ret
          = connect (socket_fd, (struct sockaddr *)&addr, sizeof (addr));
      if (connect_ret != -1)
        {
          break;
        }
    }
  while (true)
    {
      int c;
      if (scanf ("%d", &c) == EOF)
        break;
      if (write (socket_fd, &c, sizeof (c)) <= 0)
        break;
      int a;
      if (read (socket_fd, &a, sizeof (a)) <= 0)
        break;
      printf ("%d\n", a);
    }
  shutdown (socket_fd, SHUT_RDWR);
  close (socket_fd);
  return 0;
}