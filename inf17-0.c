
#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

size_t
read_data_and_count (size_t N, int in[])
{
  int epoll_fd = epoll_create1 (0);
  for (size_t i = 0; i < N; ++i)
    {
      fcntl (in[i], F_SETFL, fcntl (in[i], F_GETFL) | O_NONBLOCK);
      struct epoll_event event
          = { .events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET,
              .data = { .u32 = i } };
      epoll_ctl (epoll_fd, EPOLL_CTL_ADD, in[i], &event);
    }
  int still_opened = N;
  size_t all_readen_bytes = 0;
  while (still_opened > 0)
    {
      struct epoll_event event;
      int epoll_ret = epoll_wait (epoll_fd, &event, 1, 1000);
      if (epoll_ret <= 0)
        {
          continue;
        }
      int i = event.data.u32;
      char buf[100];
      int read_bytes = 0;
      while ((read_bytes = read (in[i], buf, sizeof (buf))) > 0)
        {
          all_readen_bytes += read_bytes;
          buf[read_bytes] = '\0';
        }
      if (read_bytes == 0)
        {
          epoll_ctl (epoll_fd, EPOLL_CTL_DEL, in[i], NULL);
          close (in[i]);
          in[i] = -1;
          --still_opened;
        }
    }
    close(epoll_fd);
    return all_readen_bytes;
}
