#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  int port = strtol (argv[1], NULL, 10);
  int socket_fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port);
  inet_aton ("127.0.0.1", &addr.sin_addr);
  int a;
  int b;
  while (scanf ("%d", &a) >= 0)
    {
      sendto (socket_fd, &a, sizeof (int), 0, (const struct sockaddr *)&addr,
              sizeof (addr));
      int size;
      recvfrom (socket_fd, &b, sizeof (int), MSG_WAITALL,
                (struct sockaddr *)&addr, &size);
      printf ("%d\n", b);
      fflush (stdout);
    }
  close (socket_fd);
  return 0;
}