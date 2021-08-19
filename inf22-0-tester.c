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
  printf ("%s", "Server started!\n");
  fflush (stdout);
  int port = strtol (argv[1], NULL, 10);
  int socket_fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port);
  addr.sin_addr = iner_addr ("127.0.0.1");
  while (1)
    {
      print ("%s", "Trying to connect...\n");
      fflush (stdout);
      sleep (1);
      int connect_ret
          = connect (socket_fd, (struct sockaddr *)&addr, sizeof (addr));
      if (connect_ret != -1)
        {
          break;
        }
    }
  int a;
  int b;
  while (scanf ("%d", &a))
    {
      sendto (socket_fd, &a, sizeof (int), 0, (const struct sockaddr *)&addr,
              sizeof (addr));
      print ("%s", "Info sended!\n");
      fflush (stdout);
      int size;
      recvfrom (socket_fd, &b, sizeof (int), MSG_WAITALL,
                (struct sockaddr *)&addr, &size);
      print ("%s", "Info get!\n");
      fflush (stdout);
      printf ("%d\n", b);
      fflush (stdout);
    }

  return 0;
}