#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

const int MAX_SIZE = 1024 * 1024;

int
main (int argc, char *argv[])
{
  char *name = argv[1];
  char *path = argv[2];

  struct addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_STREAM };

  struct addrinfo *server_info;
  getaddrinfo (name, "http", &hints, &server_info);

  int socket_fd = socket (AF_INET, SOCK_STREAM, 0);

  int connect_ret
      = connect (socket_fd, server_info->ai_addr, server_info->ai_addrlen);

  char request[MAX_SIZE];
  snprintf (request, sizeof (request),
            "GET %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "User-Agent: Mozilla/5.0\r\n"
            "Connection: close\r\n\r\n",
            path, name);
  write (socket_fd, request, sizeof (request));

  FILE *file_fd = fdopen (socket_fd, "r+");
  char header_str[MAX_SIZE];

  while (fgets (header_str, sizeof (header_str), file_fd))
    {
      if (strcmp (header_str, "\r\n") == 0)
        {
          break;
        }
    }

  while (fgets (header_str, sizeof (header_str), file_fd))
    {
      write (1, header_str, strlen (header_str));
    }

  shutdown (socket_fd, O_RDWR);
  fclose (file_fd);

  return 0;
}