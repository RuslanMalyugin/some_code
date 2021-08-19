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

void
CommunicateWithSocket (int socket_fd)
{
  while (true)
    {
      int value_from_server = 0;
      int value_to_server = 0;
      scanf ("%d", &value_to_server);
      int written = write (socket_fd, &value_to_server, sizeof (int));
      if (written <= 0)
        {
          break;
        }
      int read_ = read (socket_fd, &value_from_server, sizeof (int));
      if (read_ <= 0)
        {
          break;
        }
      printf ("%d\n", value_from_server);
    }
}

void
ClientFunction (const char *host_name, const int port)
{
  int socket_fd = socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port);
  inet_aton (host_name, &addr.sin_addr);

  while (true)
    {
      int connect_ret
          = connect (socket_fd, (struct sockaddr *)&addr, sizeof (addr));
      if (connect_ret != -1)
        {
          break;
        }
      sleep (1);
    }
  // CommunicateWithSocket(socket_fd);
  char text[] = "ABOBA\n";
  //scanf("%s", text);
  int len = strlen(text);
  write (socket_fd, text, 6);
  write (socket_fd, text, 6);
  char buffer[300] = { '\0' };
  read (socket_fd, buffer, sizeof (buffer) - 1);
  printf ("%s\n", buffer);
  fflush (stdout);
  shutdown (socket_fd, SHUT_RDWR);
  close (socket_fd);
}

int
main (int argc, char *argv[])
{
  uint16_t port_num = strtoul (argv[2], NULL, 10);
  ClientFunction (argv[1], port_num);
  return 0;
}