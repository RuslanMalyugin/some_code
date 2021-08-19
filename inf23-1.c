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
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

const int MAX_SIZE = 4096;

int
main (int argc, char *argv[])
{
  char *name = argv[1];
  char *path = argv[2];
  char *file = argv[3];
  struct addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_STREAM };

  struct addrinfo *server_info;
  getaddrinfo (name, "http", &hints, &server_info);

  int socket_fd = socket (AF_INET, SOCK_STREAM, 0);

  int connect_ret
      = connect (socket_fd, server_info->ai_addr, server_info->ai_addrlen);

  int file_fd = open (file, 0644);

  struct stat st;
  stat (file, &st);
  int64_t sz = st.st_size;

  char request[MAX_SIZE];
  int size = snprintf (request, sizeof (request),
                       "POST %s HTTP/1.1\r\n"
                       "Host: %s\r\n"
                       "Content-Type: multipart/form-data\r\n"
                       "User-Agent: Mozilla/5.0\r\n"
                       "Connection: close\r\n"
                       "Content-Length: %ld\r\n\r\n",
                       path, name, sz);

  write (socket_fd, request, size);

  sendfile (socket_fd, file_fd, NULL, sz);

  write (socket_fd, "\r\n", 2);

  FILE *sock_file = fdopen (socket_fd, "r+");

  char header_str[MAX_SIZE];

  while (fgets (header_str, sizeof (header_str), sock_file))
    {
      if (strcmp (header_str, "\r\n") == 0)
        {
          break;
        }
    }

  while (fgets (header_str, sizeof (header_str), sock_file))
    {
      write (1, header_str, strlen (header_str));
    }

  shutdown (socket_fd, O_RDWR);
  fclose (sock_file);
  close (file_fd);
  return 0;
}