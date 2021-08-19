#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef enum
{
  PERRROR = 1,
  ERROR = -1,
  BUFFER_SIZE = 8192,
  MEGA_BUFFER_SIZE = 1000000,
  HTTP_PORT = 80
} constants;

int
ConnectTo (char *server_name)
{
  signal (SIGPIPE, SIG_IGN);
  struct addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_STREAM };
  struct addrinfo *result = NULL;
  if (getaddrinfo (server_name, "http", &hints, &result))
    {
      perror ("Failed to get addr\n");
      exit (PERRROR);
    }

  int socket_fd = socket (AF_INET, SOCK_STREAM, 0);
  if (connect (socket_fd, result->ai_addr, result->ai_addrlen))
    {
      perror ("Failed to get connect\n");
      exit (PERRROR);
    }

  return socket_fd;
}

FILE *
HTTPGet (int socket_fd, char *server_name, char *path_to_file)
{

  char request[BUFFER_SIZE];
  snprintf (request, sizeof (request),
            "GET %s HTTP/1.1\n"
            "Host: %s\n"
            "User-Agent: Mozilla/5.0\n"
            "Connection: close\n\n",
            path_to_file, server_name);

  write (socket_fd, request, strnlen (request, sizeof (request)));

  return fdopen (socket_fd, "r");
}

void
PrintHTTPFile (FILE *file)
{
  char buffer[MEGA_BUFFER_SIZE];
  // skip header
  while (fgets (buffer, sizeof (buffer), file))
    {
      if (strcmp (buffer, "\r\n") == 0)
        {
          break;
        }
    }
  while (fgets (buffer, sizeof (buffer), file))
    {
      printf ("%s", buffer);
    }
}

int
main (int argc, char *argv[])
{
  char *server_name = argv[1];
  char *path_to_file = argv[2];
  if (argc != 3)
    {
      printf ("Invalid count of arguments\n");
    }

  int socket_fd = ConnectTo (server_name);
  FILE *file_http = HTTPGet (socket_fd, server_name, path_to_file);
  PrintHTTPFile (file_http);

  fclose (file_http);
}