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

void
read_and_write (int fd, char directory[])
{
  char get[4096];
  char file_name[4096];
  char http[4096];
  FILE *fd_ = fdopen (fd, "r+");
  fscanf (fd_, "%s", get);
  fscanf (fd_, "%s", file_name);
  fscanf (fd_, "%s", http);
  char current_string[4096];
  while (true)
    {
      if (!fgets (current_string, 4096, fd_))
        break;
      if ((current_string[0] == '\r') && (current_string[1] == '\n'))
        {
          break;
        }
    }
  strcat (directory, "/");
  strcat (directory, file_name);
  int exist_checker = access (directory, F_OK);
  if (exist_checker == 0)
    {
      int access_checker = access (directory, R_OK);
      if (access_checker == 0)
        {
          int exec_checker = access (directory, X_OK);
          if (exec_checker == 0)
            {
              write (fd, "HTTP/1.1 200 OK\r\n", 17);
              //fprintf(fd_, "%s", "hello!");
              int pid = fork ();
              if (pid == 0)
                {
                  dup2 (fd, 1);
                  execlp (directory, directory, NULL);
                }
              int status = 0;
              waitpid (pid, &status, 0);
            }
          else
            {
              FILE *current_file = fopen (directory, "r");
              fseek (current_file, 0L, SEEK_END);
              int sz = ftell (current_file);
              rewind (current_file);
              char file_as_is[sz];
              fread (file_as_is, 1, sz, current_file);
              write (fd, "HTTP/1.1 200 OK\r\n", 17);
              char output[128];
              int size = snprintf (output, 128, "Content-Length: %d\r\n", sz);
              write (fd, output, size);
              fread (file_as_is, 1, sz, current_file);
              write (fd, "\r\n", 2);
              write (fd, file_as_is, sz);
            }
        }
      else
        {
          char output[]
              = "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n\r\n";
          write (fd, output, 47);
        }
    }
  else
    {
      char *output = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n\r\n";
      write (fd, output, 47);
    }
}

int
server_main (int argc, char **argv, int stop_fd)
{
  assert (argc >= 2);
  char *directory = argv[2];
  uint16_t PORT = strtoul (argv[1], NULL, 10);
  int socket_fd = socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr
      = { .sin_family = AF_INET, .sin_port = htons (PORT), .sin_addr = 0 };
  bind (socket_fd, (struct sockaddr *)&addr, sizeof (addr));
  listen (socket_fd, 4096);
  int epoll_fd = epoll_create1 (0);
  {
    int fds[] = { stop_fd, socket_fd, -1 };
    for (int *fd = fds; *fd != -1; ++fd)
      {
        struct epoll_event event = { .events = EPOLLIN | EPOLLERR | EPOLLHUP,
                                     .data = { .fd = *fd } };
        epoll_ctl (epoll_fd, EPOLL_CTL_ADD, *fd, &event);
      }
  }
  while (true)
    {
      struct epoll_event event;
      int epoll_ret = epoll_wait (epoll_fd, &event, 1, 1000);
      if (epoll_ret <= 0)
        {
          continue;
        }
      if (event.data.fd == stop_fd)
        {
          break;
        }
      struct sockaddr_in peer_addr = { 0 };
      socklen_t peer_addr_size = sizeof (struct sockaddr_in);
      int connection_fd
          = accept (socket_fd, (struct sockaddr *)&peer_addr, &peer_addr_size);

      read_and_write (connection_fd, directory);
      shutdown (connection_fd, SHUT_RDWR);
      close (connection_fd);
      break;
    }
  close (epoll_fd);
  shutdown (socket_fd, SHUT_RDWR);
  close (socket_fd);
  return 0;
}

static int stop_pipe_fds[2] = { -1, -1 };

static void
stop_signal_handler (int signum)
{
  write (stop_pipe_fds[1], "X", 1);
}

int
main (int argc, char **argv)
{

  pipe (stop_pipe_fds);
  fcntl (stop_pipe_fds[1], F_SETFL,
         fcntl (stop_pipe_fds[1], F_GETFL, 0) | O_NONBLOCK);

  int signals[] = { SIGINT, SIGTERM, 0 };
  for (int *signal = signals; *signal; ++signal)
    {
      sigaction (*signal,
                 &(struct sigaction){ .sa_handler = stop_signal_handler,
                                      .sa_flags = SA_RESTART },
                 NULL);
    }

  int ret = server_main (argc, argv, stop_pipe_fds[0]);
  close (stop_pipe_fds[0]);
  close (stop_pipe_fds[1]);
  return ret;
}