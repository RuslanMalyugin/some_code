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
read_and_write (int fd)
{
  char buf[4096];
  int all_readen_bytes = 0;

  while (true)
    {
      all_readen_bytes = read (fd, buf, sizeof (buf));
      if (all_readen_bytes <= 0)
        {
          shutdown (fd, SHUT_RDWR);
          close (fd);
          break;
        }
      else
        {
          for (int i = 0; i < all_readen_bytes; ++i)
            {
              buf[i] = toupper (buf[i]);
            }
          write (fd, buf, all_readen_bytes);
        }
    }
}

int
server_main (int argc, char **argv, int stop_fd)
{
  uint16_t PORT = strtoul (argv[1], NULL, 10);
  int socket_fd = socket (AF_INET, SOCK_STREAM, 0);
  fcntl (socket_fd, F_SETFL, fcntl (socket_fd, F_GETFL) | O_NONBLOCK);
  struct sockaddr_in addr
      = { .sin_family = AF_INET, .sin_port = htons (PORT), .sin_addr = 0 };
  bind (socket_fd, (struct sockaddr *)&addr, sizeof (addr));
  listen (socket_fd, SOMAXCONN);
  int epoll_fd = epoll_create1 (0);
  {
    int fds[] = { stop_fd, socket_fd, -1 };
    for (int *fd = fds; *fd != -1; ++fd)
      {
        struct epoll_event event
            = { .events = EPOLLIN | EPOLLHUP, .data = { .fd = *fd } };
        epoll_ctl (epoll_fd, EPOLL_CTL_ADD, *fd, &event);
      }
  }
  struct epoll_event event
      = { .events = EPOLLIN | EPOLLHUP, .data = { .fd = socket_fd } };

  struct epoll_event events[SOMAXCONN];

  int stop_checker = 1;
  int prev_fd;
  int checker_sgn = 0;
  while (stop_checker)
    {

      int epoll_ret = 0;
      epoll_ret = epoll_wait (epoll_fd, events, SOMAXCONN, 1000);

      if (epoll_ret <= 0)
        {
          continue;
        }
      for (int i = 0; i < epoll_ret; ++i)
        {
          if (events[i].data.fd == stop_fd)
            {
              if (errno == EAGAIN)
                {
                  continue;
                }
              stop_checker = 0;
              close (epoll_fd);
              shutdown (socket_fd, SHUT_RDWR);
              for (int j = 0; j < i; ++j)
                {
                  shutdown (events[j].data.fd, SHUT_RDWR);
                }
              close (socket_fd);
              break;
            }
          if (events[i].data.fd == socket_fd)
            {
              struct sockaddr_in peer_addr = { 0 };
              socklen_t peer_addr_size = sizeof (struct sockaddr_in);

              int connection_fd = accept (
                  socket_fd, (struct sockaddr *)&peer_addr, &peer_addr_size);
              fcntl (connection_fd, F_SETFL,
                     fcntl (connection_fd, F_GETFL) | O_NONBLOCK);
              event.data.fd = connection_fd;
              epoll_ctl (epoll_fd, EPOLL_CTL_ADD, connection_fd, &event);
            }
          else
            {
              read_and_write (events[i].data.fd);
            }
        }
    }

  if (stop_checker)
    {
      close (epoll_fd);
      shutdown (prev_fd, SHUT_RDWR);
      shutdown (socket_fd, SHUT_RDWR);
      close (socket_fd);
    }
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

  int signals[] = { SIGTERM, 0 };
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